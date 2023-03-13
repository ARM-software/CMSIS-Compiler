/*-----------------------------------------------------------------------------
 * Name:    retarget_os_user.c
 * Purpose: OS Interface User Template
 * Rev.:    1.0.0
 *-----------------------------------------------------------------------------*/

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

/* Mutex identifier */
typedef void *mutex;

/* Mutex function prototypes */
int  _mutex_initialize(mutex *m);
void _mutex_acquire   (mutex *m);
void _mutex_release   (mutex *m);
void _mutex_free      (mutex *m);

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
  /* .. */
  return 0;
}

/*
  Acquire mutex

  This function causes the calling thread to obtain a lock on the supplied mutex.
  _mutex_acquire() returns immediately if the mutex has no owner. If the mutex
  is owned by another thread, _mutex_acquire() must block until it becomes available.
  _mutex_acquire() is not called by the thread that already owns the mutex.
*/
void _mutex_acquire(mutex *m) {
  /* .. */
}

/*
  Release mutex

  This function causes the calling thread to release the lock on a mutex acquired
  by _mutex_acquire(). The mutex remains in existence, and can be re-locked by a
  subsequent call to mutex_acquire(). _mutex_release() assumes that the mutex is
  owned by the calling thread.
*/
void _mutex_release(mutex *m) {
  /* .. */
}

/*
  Free mutex

  This function causes the calling thread to free the supplied mutex. Any operating
  system resources associated with the mutex are freed. The mutex is destroyed and
  cannot be reused. _mutex_free() assumes that the mutex is owned by the calling thread.
*/
void _mutex_free(mutex *m) {
  /* .. */
}
