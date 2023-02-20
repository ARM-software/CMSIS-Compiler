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

/* History:
 *  Version 1.0
 *    Initial release
 */

#ifndef RETARGET_FS_H__
#define RETARGET_FS_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**** File Open Mode definitions ****/
#define FS_OPEN_RDONLY        0x0000       ///< Open file for reading only
#define FS_OPEN_WRONLY        0x0001       ///< Open file for writing only
#define FS_OPEN_RDWR          0x0002       ///< Open file for reading and writing
#define FS_OPEN_APPEND        0x0008       ///< Open file in append mode
#define FS_OPEN_CREATE        0x0100       ///< Create file if it does not exist
#define FS_OPEN_TRUNCATE      0x0200       ///< Truncate existing file

/**** File Seek Operation definitions ****/
#define FS_SEEK_SET           0            ///< Set position offset from the start of the file
#define FS_SEEK_CUR           1            ///< Set position offset from the current location
#define FS_SEEK_END           2            ///< Set position offset from the end of the file

/**
  Open a file.

  This function opens a file specified by the pathname.

  The mode parameter is a bitmap that specifies the file open mode.
  It can have the following values:
  - FS_OPEN_RDONLY: open file for reading only
  - FS_OPEN_WRONLY: open file for writing only
  - FS_OPEN_RDWR:   open file for reading and writing
  Only one of the values above shall be set in addition to any combination of the
  values below:
  - FS_OPEN_APPEND: if set, the file offset is set to the end of file prior to each write
  - FS_OPEN_CREATE: if set, the file is created if it does not exist
  - FS_OPEN_TRUNCATE: if set, the size of an existing file opened for writing is truncated to zero

  The file position offset shall be set to the beginning of the file unless append mode is specified.

  \param[in]     path  string specifying the pathname of the file to be opened
  \param[in]     mode  integer bitmap specifying the file open mode
  \return        a non-negative integer representing the file descriptor on success, or -1 on failure
*/
int32_t fs_open (const char *path, int32_t mode);

/**
  Close a file.

  This function closes the file asociated with the file descriptor fd.

  \param[in]     fd    file descriptor as obtained by fs_open
  \return        zero if the file was successfully closed, or -1 on failure
*/
int32_t fs_close (int32_t fd);

/**
  Write to a file.

  This function shall write cnt bytes from the buffer pointed to by buf to the
  file associated with the open file descriptor, fd.

  \param[in]     fd    file descriptor as obtained by fs_open
  \param[in]     buf   pointer to the buffer containing data to write
  \param[in]     cnt   number of bytes to write
  \return        number of bytes actually written, or -1 on failure
*/
int32_t fs_write (int32_t fd, const void *buf, uint32_t cnt);

/**
  Read from a file.

  This function reads cnt bytes from the file associated with the file
  descriptor, fd, into the buffer pointed to by buf.
  The actuall number of bytes read can be less than cnt.

  \param[in]     fd    file descriptor as obtained by fs_open
  \param[out]    buf   pointer to the buffer to store read data
  \param[in]     cnt   number of bytes to read
  \return        number of bytes actually read, 0 at the EOF or -1 if failure occurs
*/
int32_t fs_read (int32_t fd, void *buf, uint32_t cnt);

/**
  Move the file position pointer.

  This functions moves the file position pointer as specified with parameter pos and whence.
  Parameter whence can have the following possible values:
  - FS_SEEK_SET: set the file position pointer to offset bytes from the start of the file
  - FS_SEEK_CUR: set the file position pointer to offset bytes from the current location
  - FS_SEEK_END: set the file position pointer to offset bytes from the end of the file

  \param[in]     fd     file descriptor as obtained by fs_open
  \param[in]     offset the number of bytes to move
  \param[in]     whence file position location (FS_SEEK_SET, FS_SEEK_CUR, FS_SEEK_END)
  \return        current file position from the beginning of the file
*/
int64_t fs_seek (int32_t fd, int64_t offset, int32_t whence);

/**
  Return the size of the file in bytes.

  \param[in]     fd    file descriptor as obtained by fs_open
  \return        file size in bytes, -1 on failure
*/
int64_t fs_size (int32_t fd);

/**
  Remove a file or directory.

  If removing a directory, the directory must be empty.

  \param[in]     path  string specifying the pathname
  \return        zero if the file or directory was successfully removed, or -1 on failure
*/
int32_t fs_remove (const char *path);

/**
  Rename or move a file or directory.

  This function changes the name of a file or directory.

  If the destination exists, it must match the source in type.
  If the destination is a directory, the directory must be empty.

  \param[in]     oldpath string specifying file or directory to be renamed
  \param[in]     newpath string specifying the new pathname

  \return        zero if the file or directory was successfully renamed or moved, or -1 on failure
*/
int32_t fs_rename (const char *oldpath, const char *newpath);


#ifdef  __cplusplus
}
#endif

#endif /* RETARGET_FS_H__ */
