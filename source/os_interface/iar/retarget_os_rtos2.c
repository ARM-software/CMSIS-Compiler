/*
 * Copyright (C) 2023-2026 ARM Limited or its affiliates. All rights reserved.
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
#include "retarget_os.h"

#include "cmsis_os2.h"
#include "cmsis_compiler.h"

/* Check if the kernel has been initialized */
static uint32_t os_kernel_is_initialized (void) {
  if (osKernelGetState() > osKernelInactive) {
    return 1U;
  } else {
    return 0U;
  }
}

/* Check if the kernel is running */
static uint32_t os_kernel_is_running (void) {
  if (osKernelGetState() == osKernelRunning) {
    return 1U;
  } else {
    return 0U;
  }
}

/* Check if processor is in Thread or Handler mode */
static uint32_t is_thread_mode (void) {
  if (__get_IPSR() == 0U) {
    return 1U; /* Thread mode  */
  } else {
    return 0U; /* Handler mode */
  }
}

/* Define retarget mutex structure to hold mutex identifier */
typedef struct {
  osMutexId_t id;
} rt_mutex_t;

#pragma language=save
#pragma language=extended

/* Initialize a system lock (create new mutex) */
__USED void __iar_system_Mtxinit(__iar_Rmtx *lock) {
  rt_mutex_t *mutex = (rt_mutex_t *)lock;
  osMutexAttr_t attr = {
    .attr_bits = osMutexRecursive
  };

  if (os_kernel_is_initialized()) {
    mutex->id = osMutexNew(&attr);
  }
}

/* Lock a system lock (acquire mutex) */
__USED void __iar_system_Mtxlock(__iar_Rmtx *lock) {
  rt_mutex_t *mutex = (rt_mutex_t *)lock;

  if (os_kernel_is_running() && is_thread_mode()) {
    (void)osMutexAcquire(mutex->id, osWaitForever);
  }
}

/*  Unlock a system lock (release mutex) */
__USED void __iar_system_Mtxunlock(__iar_Rmtx *lock) {
  rt_mutex_t *mutex = (rt_mutex_t *)lock;

  if (os_kernel_is_running() && is_thread_mode()) {
    (void)osMutexRelease(mutex->id);
  }
}

/* Destroy a system lock (delete mutex) */
__USED void __iar_system_Mtxdst(__iar_Rmtx *lock) {
  rt_mutex_t *mutex = (rt_mutex_t *)lock;

  (void)osMutexDelete(mutex->id);
}

/* Initialize a file lock (create new mutex) */
__USED void __iar_file_Mtxinit(__iar_Rmtx *lock) {
  rt_mutex_t *mutex = (rt_mutex_t *)lock;
  osMutexAttr_t attr = {
    .attr_bits = osMutexRecursive
  };

  if (os_kernel_is_initialized()) {
    mutex->id = osMutexNew(&attr);
  }
}

/* Lock a file lock (acquire mutex) */
__USED void __iar_file_Mtxlock(__iar_Rmtx *lock) {
  rt_mutex_t *mutex = (rt_mutex_t *)lock;

  if (os_kernel_is_running() && is_thread_mode()) {
    (void)osMutexAcquire(mutex->id, osWaitForever);
  }
}

/* Unlock a file lock (release mutex) */
__USED void __iar_file_Mtxunlock(__iar_Rmtx *lock) {
  rt_mutex_t *mutex = (rt_mutex_t *)lock;

  if (os_kernel_is_running() && is_thread_mode()) {
    (void)osMutexRelease(mutex->id);
  }
}

/* Destroy a file lock (delete mutex) */
__USED void __iar_file_Mtxdst(__iar_Rmtx *lock) {
  rt_mutex_t *mutex = (rt_mutex_t *)lock;

  (void)osMutexDelete(mutex->id);
}

#pragma language=restore
