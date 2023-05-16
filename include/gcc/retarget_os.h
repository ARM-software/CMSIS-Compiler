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
  The unistd.h header file contains declarations for types, constants and functions
  that provide access to system services.

  It defines functions that perform operations like file I/O, process management and
  inter-process communication.
*/
#include <unistd.h>

/*
  The fcntl.h header file contains declarations for functions, constants, and types
  that provide access to operations on file descriptors.
*/
#include <fcntl.h>

/*
  The time.h header file provides declarations for types, macros, and functions
  for manipulating time.

  It defines macros, types, and functions that deal with calendar time, handle
  local time expressed for a specific time zone and Daylight Savings Time.
*/
#include <time.h>

/*
  The stdio.h header file provides declarations for input and output operations.
*/
#include <stdio.h>


//  ==== Locking Routines ====

/**
  \fn void __retarget_lock_init(_LOCK_T *lock)

  \brief Allocate lock related resources.

  \param[in]  lock  pointer to user defined lock object
*/


/**
  \fn void __retarget_lock_init_recursive(_LOCK_T *lock)

  \brief Allocate recursive lock related resources.

  \param[in]  lock  pointer to user defined lock object
*/


/**
  \fn void __retarget_lock_close(_LOCK_T lock)

  \brief Free lock related resources.

  \param[in]  lock  user defined lock object
*/

/**
  \fn void __retarget_lock_close_recursive(_LOCK_T lock)

  \brief Free recursive lock related resources.

  \param[in]  lock  user defined lock object
*/

/**
  \fn void __retarget_lock_acquire(_LOCK_T lock)

  \brief Acquire lock immediately after the lock object is available.

  \param[in]  lock  user defined lock object
*/

/**
  \fn void __retarget_lock_acquire_recursive(_LOCK_T lock)

  \brief Acquire recursive lock immediately after the lock object is available.

  \param[in]  lock  user defined lock object
*/

/**
  \fn int __retarget_lock_try_acquire(_LOCK_T lock)

  \brief Acquire lock if the lock object is available.

  \param[in]  lock  user defined lock object

  \return     zero for success and non-zero to indicate that the lock cannot be acquired
*/

/**
  \fn int __retarget_lock_try_acquire_recursive(_LOCK_T lock)

  \brief Acquire recursive lock if the lock object is available.

  \param[in]  lock  user defined lock object

  \return     zero for success and non-zero to indicate that the lock cannot be acquired
*/

/**
  \fn void __retarget_lock_release(_LOCK_T lock)

  \brief Relinquish the lock ownership.

  \param[in]  lock  user defined lock object
*/

/**
  \fn void __retarget_lock_release_recursive(_LOCK_T lock)

  \brief Relinquish the recursive lock ownership.

  \param[in]  lock  user defined lock object
*/


//  ==== System Calls ====

/**
  \fn int _open (const char *path, int oflag, ...)

  \brief Open file.

  \param[in]  path      Pathname naming the file
  \param[in]  oflag     Mode specification bitmap

  \return  Upon successful completion, function opens the file and returns
           a non-negative integer representing the file descriptor.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn int _close (int fildes)

  \brief Close a file descriptor.

  \param[in]  fildes    File descriptor

  \return  Upon successful completion, 0 is returned.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn ssize_t _write (int fildes, const void *buf, size_t nbyte)

  \brief Write on a file.

  \param[in]  fildes    File descriptor
  \param[in]  buf       A pointer to data buffer
  \param[in]  nbyte     Number of bytes to write

  \return  Upon successful completion, this function returns the number
           of bytes actually written to the file associated with fildes.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn ssize_t _read (int fildes, void *buf, size_t nbyte)

  \brief Read from a file.

  \param[in]  fildes    File descriptor
  \param[out] buf       A pointer to data buffer
  \param[in]  nbyte     Number of bytes to read

  \return  Upon successful completion, function returns a non-negative integer
           indicating the number of bytes actually read.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn off_t _lseek (int fildes, off_t offset, int whence)

  \brief Move the read/write file offset.

  \param[in]  fildes    File descriptor
  \param[in]  offset    File offset
  \param[in]  whence    File seek mode

  \return  Upon successful completion, the resulting offset in bytes from the beginning
           of the file is returned.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn int _isatty (int fildes)

  \brief Test for a terminal device.

  \param[in]  fildes    File descriptor

  \return  The return value is 1 if fildes is associated with a terminal.
           Otherwise, 0 is returned.
*/

/**
  \fn int _fstat (int fildes, struct stat *buf)

  \brief Get file status.

  \param[in]  fildes    File descriptor
  \param[out] buf       A pointer to a stat structure

  \return  Upon successful completion, 0 is returned.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn int _stat (const char *path, struct stat *buf)

  \brief Get file status (by name).

  \param[in]  path      Pathname naming the file
  \param[in]  buf       A pointer to a stat structure

  \return  Upon successful completion, 0 is returned.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn int _link(const char *path1, const char *path2)

  \brief Link one file to another file.

  \param[in]  path1     Pathname naming an existing file
  \param[out] path2     Pathname naming the new directory entry to be created

  \return  Upon successful completion, 0 is returned.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn int _unlink (const char *path)

  \brief Remove a directory entry.

  \param[in]  path      Pathname naming the file

  \return  Upon successful completion, 0 is returned.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \var extern char **environ

  \brief A pointer to an array of character pointers to the environment strings.
*/

/**
  \fn int _execve(const char *path, char *const argv[], char *const envp[])

  \brief Execute a file.

  \param[in]  path      Pathname that identifies the new process image file
  \param[in]  argv      Pointers to strings that constitute process argument list
  \param[in]  envp      Pointers to strings that constitute process environment variables

  \return  If the function returns an error occured, -1 is returned and errno
           is set to indicate the error.
*/

/**
  \fn pid_t fork (void)

  \brief Create a new process.

  \return  Upon successful completion:
           - 0 is returned to the child process.
           - process ID of the child process is returned to the parent process.
           Otherwise, -1 is returned and sets errno to indicate the error.
*/

/**
  \fn void _exit (int status)

  \brief Terminate a process.

  \param[in]  status    Process exit status code
*/

/**
  \fn int _kill (pid_t pid, int sig)

  \brief Send a signal to a process or a group of processes.

  \return  Upon successful completion, 0 is returned.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn pid_t _getpid (void)

  \brief Get the process ID.

  \return  Process ID (function is always successful).
*/

/**
  \fn pid_t _wait (int *stat_loc)

  \brief Wait for a child process to stop or terminate.

  \param[in]  stat_loc  Pointer to a integer to store status information

  \return  If function returns because the status of a child process is available,
           the child process ID is returned.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

/**
  \fn void *_sbrk (ptrdiff_t incr)

  \brief Extend heap space by incr bytes.

  \param[in]  incr      The number of bytes to increment the heap by

  \return  Upon successful completion, a pointer to the start of the new block
           of memory is returned.
           Otherwise, the function returns -1 and sets errno to indicate the error.
*/

#endif /* RETARGET_OS_H__ */
