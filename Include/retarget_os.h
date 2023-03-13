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

#ifndef RETARGET_OS_H__
#define RETARGET_OS_H__

/*
  The rt_heap.h header file defines the interface for retargeting the heap implementation.

  It provides a way for users to customize the way that the heap is managed, by
  defining their own functions for allocating and deallocating memory.
*/
#include <rt_heap.h>

/*
  The rt_locale.h header file enables the retargetting of the locale mechanism.

  It is used to override the default behavior of the locale-related functions
  and to provide custom locale functionality.
*/
#include <rt_locale.h>

/*
  The rt_misc.h header file contains definitions for various retargetable functions.

  It includes functions like _getenv_init(), _clock_init(), __user_libspace(),
  __rt_lib_init(), __ARM_get_argv(), __rt_lib_shutdown(), __rt_exit(),
  __user_initial_stackheap(), __user_heap_extent() and __user_stack_slop() which are
  responsible for the initialization, shutdown, and memory allocation of various
  parts of the library.

  The header file also includes various structures like __initial_stackheap,
  __heap_extent, and __stack_slop, which can be defined by users to give bounds
  on the heap and stack address space.
*/
#include <rt_misc.h>

/*
  The time.h header file provides declarations for types, macros, and functions
  for manipulating time.

  It defines macros, types, and functions that deal with calendar time, handle
  local time expressed for a specific time zone and Daylight Savings Time.
*/
#include <time.h>

/*
  Retrieve thread local storage.

  This function returns a pointer to memory for storing data that is local to a
  particular thread. This means that __user_perthread_libspace() returns a
  different address depending on the thread it is called from.
*/
void *__user_perthread_libspace (void);

/* Mutex identifier */
typedef void *mutex;

/*
  Initialize mutex.

  This function accepts a pointer to a pointer-sized word and initializes it as
  a valid mutex. By default, _mutex_initialize() returns zero for a nonthreaded
  application. Therefore, in a multithreaded application, _mutex_initialize()
  must return a nonzero value on success so that at runtime, the library knows
  that it is being used in a multithreaded environment.
  Ensure that _mutex_initialize() initializes the mutex to an unlocked state.
*/
int _mutex_initialize(mutex *m);

/*
  Acquire mutex.

  This function causes the calling thread to obtain a lock on the supplied mutex.
  _mutex_acquire() returns immediately if the mutex has no owner. If the mutex
  is owned by another thread, _mutex_acquire() must block until it becomes available.
  _mutex_acquire() is not called by the thread that already owns the mutex.
*/
void _mutex_acquire(mutex *m);

/*
  Release mutex.

  This function causes the calling thread to release the lock on a mutex acquired
  by _mutex_acquire(). The mutex remains in existence, and can be re-locked by a
  subsequent call to mutex_acquire(). _mutex_release() assumes that the mutex is
  owned by the calling thread.
*/
void _mutex_release(mutex *m);

/*
  Free mutex.

  This function causes the calling thread to free the supplied mutex. Any operating
  system resources associated with the mutex are freed. The mutex is destroyed and
  cannot be reused. _mutex_free() assumes that the mutex is owned by the calling thread.
*/
void _mutex_free(mutex *m);

#endif /* RETARGET_OS_H__ */
