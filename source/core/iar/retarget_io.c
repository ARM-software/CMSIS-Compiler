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

#include <LowLevelIOInterface.h>
#include <stdio.h>

#include "RTE_Components.h"

#ifdef RTE_CMSIS_Compiler_File_Interface
#include "retarget_fs.h"
#endif

#ifdef RTE_CMSIS_Compiler_STDERR
#include "retarget_stderr.h"
#endif

#ifdef RTE_CMSIS_Compiler_STDIN
#include "retarget_stdin.h"
#endif

#ifdef RTE_CMSIS_Compiler_STDOUT
#include "retarget_stdout.h"
#endif

#if defined(RTE_CMSIS_Compiler_STDIN) || defined(RTE_CMSIS_Compiler_File_Interface)

size_t __read(int handle, unsigned char *buf, size_t bufSize)
{
#if defined(RTE_CMSIS_Compiler_STDIN)
  if (handle == _LLIO_STDIN)
  {
    size_t nChars = 0;
    for (size_t i = bufSize; i > 0; --i)
    {
      const int c = stdin_getchar();
      if (c < 0) {
        break;
      } 
      *buf++ = (unsigned char)c;
      nChars++;
    }
    return nChars;
  }
#endif

#if defined(RTE_CMSIS_Compiler_File_Interface)
  int32_t sz = rt_fs_read(handle, buf, bufSize);
  if (sz < 0) {
    sz = -1;
  }
  return (sz);
#else
  /* Not implemented */
  return _LLIO_ERROR;
#endif
}

#endif

#if defined(RTE_CMSIS_Compiler_STDOUT) || defined(RTE_CMSIS_Compiler_STDERR) || defined(RTE_CMSIS_Compiler_File_Interface)

size_t __write(int handle, const unsigned char *buf, size_t bufSize)
{
  /* Check for the command to flush all handles */
  if (handle == -1)
  {
    return 0;
  }

#if defined(RTE_CMSIS_Compiler_STDOUT)  
  if (handle == _LLIO_STDOUT) {
    for (size_t i = bufSize; i > 0; --i)
    {
      stdout_putchar(*buf++);
    }
    return bufSize;
  }
#endif

#if defined(RTE_CMSIS_Compiler_STDERR)
  if (handle == _LLIO_STDERR) {
    for (size_t i = bufSize; i > 0; --i)
    {
      stderr_putchar(*buf);
      ++buf;
    }
    return bufSize;
  }
#endif


#if defined(RTE_CMSIS_Compiler_File_Interface)
  int32_t sz = rt_fs_write(handle, buf, bufSize);
  if (sz < 0) {
    sz = -1;
  }
  return (sz);
#else
  /* Not implemented */
  return _LLIO_ERROR;
#endif
}

#endif


int  __close(int fildes)
{
  switch (fildes) {
    case _LLIO_STDIN:
    case _LLIO_STDOUT:
    case _LLIO_STDERR:
      return (0);
  }

#if defined(RTE_CMSIS_Compiler_File_Interface)
  int32_t rval;
  rval = rt_fs_close(fildes);
  if (rval < 0) {
    rval = _LLIO_ERROR;
  }
  return (rval);
#else
  /* Not implemented */
  return _LLIO_ERROR;
#endif
}


int  __open(const char *path, int oflag)
{
#if defined(RTE_CMSIS_Compiler_File_Interface)
  int32_t rval;
  int32_t mode;
  mode  = oflag & (_LLIO_RDONLY | _LLIO_WRONLY | _LLIO_RDWR | _LLIO_APPEND);
  mode |= (oflag & (_LLIO_CREAT | _LLIO_TRUNC)) >> 1;
  rval = rt_fs_open(path, mode);
  if (rval < 0) {
    rval = _LLIO_ERROR;
  }
  return (rval);
#else
  /* Not implemented */
  (void)path;
  (void)oflag;
  return _LLIO_ERROR;
#endif
}

long __lseek(int fildes, long offset, int whence)
{
#if defined(RTE_CMSIS_Compiler_File_Interface)
  int64_t rval;
  rval = rt_fs_seek(fildes, offset, whence);
  if (rval < 0) {
    rval = _LLIO_ERROR;
  }
  else {
    if ((sizeof(long) != sizeof(int64_t)) && ((rval >> 32) != 0)) {
      /* Returned file offset does not fit into off_t */
      rval = _LLIO_ERROR;
    }
  }
  return ((long)rval);
#else
  /* Not implemented */
  (void)offset;
  (void)whence;
  return _LLIO_ERROR;
#endif
}

