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

#if !(defined(RTE_CMSIS_RTOS2_RTX5) || defined(RTE_CMSIS_RTOS2_FreeRTOS)) || defined(RTE_RETARGET_OS_Interface_RTOS2_ENABLE)

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

/* Define retarget mutex structure to hold mutex identifier */
struct rt_mutex_s {
  osMutexId_t id;
};

/* Initialize mutex */
__USED int _mutex_initialize(rt_mutex_t *mutex) {
  int result;

  mutex->id = osMutexNew(NULL);

  if (mutex->id != NULL) {
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

/* Acquire mutex */
__USED void _mutex_acquire(rt_mutex_t *mutex) {
  if (os_kernel_is_active() != 0U) {
    (void)osMutexAcquire(mutex->id, osWaitForever);
  }
}

/* Release mutex */
__USED void _mutex_release(rt_mutex_t *mutex) {
  if (os_kernel_is_active() != 0U) {
    (void)osMutexRelease(mutex->id);
  }
}

/* Free mutex */
__USED void _mutex_free(rt_mutex_t *mutex) {
  (void)osMutexDelete(mutex->id);
}
#endif
#endif
