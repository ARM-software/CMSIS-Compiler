/*-----------------------------------------------------------------------------
 * Name:    file_user.c
 * Purpose: C library File Interface User Template
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

#include "retarget_fs.h"

/**
  Open a file.

  \param[in]     path  string specifying the pathname of the file to be opened
  \param[in]     mode  integer bitmap specifying the file open mode
  \return        a non-negative integer representing the file descriptor on success, or -1 on failure
*/
int32_t fs_open (const char *path, int32_t mode) {
  return (-1);
}

/**
  Close a file.

  \param[in]     fd    file descriptor as obtained by fs_open
  \return        zero if the file was successfully closed, or -1 on failure
*/
int32_t fs_close (int32_t fd) {
  return (-1);
}

/**
  Write to a file.

  \param[in]     fd    file descriptor as obtained by fs_open
  \param[in]     buf   pointer to the buffer containing data to write
  \param[in]     cnt   number of bytes to write
  \return        number of bytes actually written, or -1 on failure
*/
int32_t fs_write (int32_t fd, const void *buf, uint32_t cnt) {
  return (-1);
}

/**
  Read from a file.

  \param[in]     fd    file descriptor as obtained by fs_open
  \param[out]    buf   pointer to the buffer to store read data
  \param[in]     cnt   number of bytes to read
  \return        number of bytes actually read, 0 at the EOF or -1 if failure occurs
*/
int32_t fs_read (int32_t fd, void *buf, uint32_t cnt) {
  return (-1);
}

/**
  Move the file position pointer.

  \param[in]     fd     file descriptor as obtained by fs_open
  \param[in]     offset the number of bytes to move
  \param[in]     whence file position location (FS_SEEK_SET, FS_SEEK_CUR, FS_SEEK_END)
  \return        current file position from the beginning of the file
*/
int64_t fs_seek (int32_t fd, int64_t offset, int32_t whence) {
  return (-1);
}

/**
  Return the size of the file in bytes.

  \param[in]     fd    file descriptor as obtained by fs_open
  \return        file size in bytes, -1 on failure
*/
int64_t fs_size (int32_t fd) {
  return (-1);
}

/**
  Remove a file or directory.

  \param[in]     path  string specifying the pathname
  \return        zero if the file or directory was successfully removed, or -1 on failure
*/
int32_t fs_remove (const char *path) {
  return (-1);
}

/**
  Rename or move a file or directory.

  \param[in]     oldpath string specifying file or directory to be renamed
  \param[in]     newpath string specifying the new pathname

  \return        zero if the file or directory was successfully renamed or moved, or -1 on failure
*/
int32_t fs_rename (const char *oldpath, const char *newpath) {
  return (-1);
}
