/*
 * Copyright (C) 2023 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include "retarget_os.h"

#include "cmsis_os2.h"
#include "cmsis_compiler.h"

#include "RTE_Components.h"

#if (( defined(__CC_ARM) ||                                          \
      (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))) && \
      !defined(__MICROLIB))

#ifndef RTE_CMSIS_RTOS2_RTX5

/* Number of Threads which use standard C/C++ library libspace */
#ifndef OS_THREAD_LIBSPACE_NUM
#define OS_THREAD_LIBSPACE_NUM      4
#endif

#define LIBSPACE_SIZE 96


/* Memory for libspace */
static uint32_t os_libspace[OS_THREAD_LIBSPACE_NUM+1][LIBSPACE_SIZE/4] \
__attribute__((section(".bss.os.libspace")));

/* Thread IDs for libspace */
static osThreadId_t os_libspace_id[OS_THREAD_LIBSPACE_NUM] \
__attribute__((section(".bss.os.libspace")));

/* Check if Kernel has been started */
static uint32_t os_kernel_is_active (void) {
  static uint8_t os_kernel_active = 0U;

  if (os_kernel_active == 0U) {
    if (osKernelGetState() > osKernelReady) {
      os_kernel_active = 1U;
    }
  }
  return (uint32_t)os_kernel_active;
}

/* Per-thread libspace retrieve function prototype */
void *__user_perthread_libspace (void);

/*
  Retrieve thread local storage

  This function returns a pointer to memory for storing data that is local to a
  particular thread. This means that __user_perthread_libspace() returns a
  different address depending on the thread it is called from.
*/
void *__user_perthread_libspace (void) {
  osThreadId_t id;
  uint32_t     n;
  void        *libspace;

  if (os_kernel_is_active() != 0U) {
    libspace = NULL;
    id = osThreadGetId();

    for (n = 0U; n < (uint32_t)OS_THREAD_LIBSPACE_NUM; n++) {
      if (os_libspace_id[n] == NULL) {
        os_libspace_id[n] = id;
      }
      if (os_libspace_id[n] == id) {
        libspace = &os_libspace[n][0];
        break;
      }
    }
  } else {
    libspace = &os_libspace[OS_THREAD_LIBSPACE_NUM][0];
  }

  assert (libspace != NULL);

  return libspace;
}

/* Mutex identifier */
typedef void *mutex;

/* Mutex function prototypes */
__USED int  _mutex_initialize(mutex *m);
__USED void _mutex_acquire   (mutex *m);
__USED void _mutex_release   (mutex *m);
__USED void _mutex_free      (mutex *m);


/*
  Initialize mutex

  This function accepts a pointer to a pointer-sized word and initializes it as
  a valid mutex. By default, _mutex_initialize() returns zero for a nonthreaded
  application. Therefore, in a multithreaded application, _mutex_initialize()
  must return a nonzero value on success so that at runtime, the library knows
  that it is being used in a multithreaded environment.
  Ensure that _mutex_initialize() initializes the mutex to an unlocked state.
*/
int _mutex_initialize(mutex *m) {
  int result;

  *m = osMutexNew(NULL);
  if (*m != NULL) {
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

/*
  Acquire mutex

  This function causes the calling thread to obtain a lock on the supplied mutex.
  _mutex_acquire() returns immediately if the mutex has no owner. If the mutex
  is owned by another thread, _mutex_acquire() must block until it becomes available.
  _mutex_acquire() is not called by the thread that already owns the mutex.
*/
void _mutex_acquire(mutex *m) {
  if (os_kernel_is_active() != 0U) {
    (void)osMutexAcquire(*m, osWaitForever);
  }
}

/*
  Release mutex

  This function causes the calling thread to release the lock on a mutex acquired
  by _mutex_acquire(). The mutex remains in existence, and can be re-locked by a
  subsequent call to mutex_acquire(). _mutex_release() assumes that the mutex is
  owned by the calling thread.
*/
void _mutex_release(mutex *m) {
  if (os_kernel_is_active() != 0U) {
    (void)osMutexRelease(*m);
  }
}

/*
  Free mutex

  This function causes the calling thread to free the supplied mutex. Any operating
  system resources associated with the mutex are freed. The mutex is destroyed and
  cannot be reused. _mutex_free() assumes that the mutex is owned by the calling thread.
*/
void _mutex_free(mutex *m) {
  (void)osMutexDelete(*m);
}
#endif /* RTE_CMSIS_RTOS2_RTX5 */
#endif
