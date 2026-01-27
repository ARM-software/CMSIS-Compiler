/*
 * Copyright (C) 2026 ARM Limited or its affiliates. All rights reserved.
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
  The DLib_Threads.h header file defines the IAR multithreading support.

  It provides interface for variables placed in thread-local storage (TLS) and
  basic locks using mutex functions. Two kinds of locks are used - system locks
  and file stream locks. The file stream locks are used when the state of a
  file stream is updated (only in the Full library configuration). The system
  locks are used to guard heap and the C library internal states.

  Note: To enable multithread support, use the linker option ‑‑threaded_lib.
*/
#include <DLib_Threads.h>

// ==== LOCKS ====

/// \fn void __iar_system_Mtxinit(__iar_Rmtx *lock)
/// \brief Initialize a system lock
/// \param[in]  lock  pointer to user defined lock object

/// \fn void __iar_system_Mtxdst(__iar_Rmtx *lock)
/// \brief Destroy a system lock
/// \param[in]  lock  pointer to user defined lock object

/// \fn void __iar_system_Mtxlock(__iar_Rmtx *lock)
/// \brief Lock a system lock
/// \param[in]  lock  pointer to user defined lock object

/// \fn void __iar_system_Mtxunlock(__iar_Rmtx *lock)
/// \brief Unlock a system lock
/// \param[in]  lock  pointer to user defined lock object

/// \fn void __iar_file_Mtxinit(__iar_Rmtx *lock)
/// \brief Initialize a file lock
/// \param[in]  lock  pointer to user defined lock object

/// \fn void __iar_file_Mtxdst(__iar_Rmtx *lock)
/// \brief Destroy a file lock
/// \param[in]  lock  pointer to user defined lock object

/// \fn void __iar_file_Mtxlock(__iar_Rmtx *lock)
/// \brief Lock a file lock
/// \param[in]  lock  pointer to user defined lock object

/// \fn void __iar_file_Mtxunlock(__iar_Rmtx *lock)
/// \brief Unlock a file lock
/// \param[in]  lock  pointer to user defined lock object

#endif /* RETARGET_OS_H__ */
