/*-----------------------------------------------------------------------------
 * Name:    retarget_syscalls.c
 * Purpose: Retarget System Calls
 * Rev.:    1.0.1
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

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "cmsis_compiler.h"

#include "RTE_Components.h"

#ifdef RTE_Compiler_OS_Interface
#include "retarget_os.h"
#endif

#if defined(RTE_Compiler_IO_File) && defined(RTE_Compiler_IO_File_Interface)
#include "retarget_fs.h"
#endif

#ifdef RTE_Compiler_IO_STDOUT_EVR
#include "EventRecorder.h"
#endif

#ifndef STDIN_ECHO
#define STDIN_ECHO      0       /* STDIN: echo to STDOUT */
#endif
#ifndef STDOUT_CR_LF
#define STDOUT_CR_LF    0       /* STDOUT: add CR for LF */
#endif
#ifndef STDERR_CR_LF
#define STDERR_CR_LF    0       /* STDERR: add CR for LF */
#endif

#if defined(RTE_Compiler_IO_File) && defined(RTE_Compiler_IO_File_Interface)
/* Local function that converts between time formats */
static void rt_to_stat_tim(rt_fs_time_t *rt_t, struct timespec *st_t);
#endif

#if (defined(RTE_Compiler_IO_STDIN_ITM)  || \
     defined(RTE_Compiler_IO_STDOUT_ITM) || \
     defined(RTE_Compiler_IO_STDERR_ITM))

/* ITM registers */
#define ITM_PORT0_U8          (*((volatile uint8_t  *)0xE0000000))
#define ITM_PORT0_U32         (*((volatile uint32_t *)0xE0000000))
#define ITM_TER               (*((volatile uint32_t *)0xE0000E00))
#define ITM_TCR               (*((volatile uint32_t *)0xE0000E80))

#define ITM_TCR_ITMENA_Msk    (1UL << 0)

/*!< Value identifying \ref ITM_RxBuffer is ready for next character. */
#define ITM_RXBUFFER_EMPTY    0x5AA55AA5

/*!< Variable to receive characters. */
extern
volatile int32_t ITM_RxBuffer;
volatile int32_t ITM_RxBuffer = ITM_RXBUFFER_EMPTY;

/** \brief  ITM Send Character

    The function transmits a character via the ITM channel 0, and
    \li Just returns when no debugger is connected that has booked the output.
    \li Is blocking when a debugger is connected, but the previous character
        sent has not been transmitted.

    \param [in]     ch  Character to transmit.

    \returns            Character to transmit.
 */
int32_t ITM_SendChar (int32_t ch);
int32_t ITM_SendChar (int32_t ch) {
  if ((ITM_TCR & ITM_TCR_ITMENA_Msk) && /* ITM enabled */
      (ITM_TER & (1UL << 0)        )) { /* ITM Port #0 enabled */
    while (ITM_PORT0_U32 == 0);
    ITM_PORT0_U8 = (uint8_t)ch;
  }
  return (ch);
}

/** \brief  ITM Receive Character

    The function inputs a character via the external variable \ref ITM_RxBuffer.
    This variable is monitored and altered by the debugger to provide input.

    \return             Received character.
    \return         -1  No character pending.
 */
int32_t ITM_ReceiveChar (void);
int32_t ITM_ReceiveChar (void) {
  int32_t ch = -1;                      /* no character available */

  if (ITM_RxBuffer != ITM_RXBUFFER_EMPTY) {
    ch = ITM_RxBuffer;
    ITM_RxBuffer = ITM_RXBUFFER_EMPTY;  /* ready for next character */
  }

  return (ch);
}

#endif  /* RTE_Compiler_IO_STDxxx_ITM */


/**
  Get a character from the stdio

  \return     The next character from the input, or -1 on read error.
*/
#if   defined(RTE_Compiler_IO_STDIN)
#if   defined(RTE_Compiler_IO_STDIN_User)
extern int stdin_getchar (void);
#elif defined(RTE_Compiler_IO_STDIN_ITM)
static int stdin_getchar (void) {
  int32_t ch;

  do {
    ch = ITM_ReceiveChar();
  } while (ch == -1);
  return (ch);
}
#elif defined(RTE_Compiler_IO_STDIN_BKPT)
static int stdin_getchar (void) {
  int32_t ch = -1;

  __asm("BKPT 0");
  return (ch);
}
#endif
#endif


/**
  Put a character to the stdout

  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
#if   defined(RTE_Compiler_IO_STDOUT)
#if   defined(RTE_Compiler_IO_STDOUT_User)
extern int stdout_putchar (int ch);
#elif defined(RTE_Compiler_IO_STDOUT_ITM)
static int stdout_putchar (int ch) {
  return (ITM_SendChar(ch));
}
#elif defined(RTE_Compiler_IO_STDOUT_EVR)
static int stdout_putchar (int ch) {
  static uint32_t index = 0U;
  static uint8_t  buffer[8];

  if (index >= 8U) {
    return (-1);
  }
  buffer[index++] = (uint8_t)ch;
  if ((index == 8U) || (ch == '\n')) {
    EventRecordData(EventID(EventLevelOp, 0xFE, 0x00), buffer, index);
    index = 0U;
  }
  return (ch);
}
#elif defined(RTE_Compiler_IO_STDOUT_BKPT)
static int stdout_putchar (int ch) {
  __asm("BKPT 0");
  return (ch);
}
#endif
#endif


/**
  Put a character to the stderr

  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
#if   defined(RTE_Compiler_IO_STDERR)
#if   defined(RTE_Compiler_IO_STDERR_User)
extern int stderr_putchar (int ch);
#elif defined(RTE_Compiler_IO_STDERR_ITM)
static int stderr_putchar (int ch) {
  return (ITM_SendChar(ch));
}
#elif defined(RTE_Compiler_IO_STDERR_BKPT)
static int stderr_putchar (int ch) {
  __asm("BKPT 0");
  return (ch);
}
#endif
#endif


/**
  Open file.

  The open() function establishes the connection between a file and a file descriptor.
  It creates an open file description that refers to a file and a file descriptor
  that refers to that open file description.
  The file descriptor is used by other I/O functions to refer to that file.
  The path argument points to a pathname naming the file.

  \param[in] path     File name
  \param[in] oflag    Mode specification bitmap

  \return    Upon successful completion, function opens the file and returns
             a non-negative integer representing the file descriptor.
             Otherwise, the function returns -1 and sets errno to indicate the error.
*/
__attribute__((weak))
int _open (const char *path, int oflag, ...) {
#if (defined(RTE_Compiler_IO_File) && defined(RTE_Compiler_IO_File_Interface))
  int32_t rval;
#else
  (void)path;
  (void)oflag;
#endif

#if defined(RTE_Compiler_IO_File)
#if defined(RTE_Compiler_IO_File_Interface)
  rval = rt_fs_open(path, oflag);
  if (rval < 0) {
    errno = rval;
    rval = -1;
  }
  return (rval);
#elif defined(RTE_Compiler_IO_File_BKPT)
  __asm("BKPT 0");
  return (-1);
#endif
#else
  return (-1);
#endif
}


/**
  Close a file descriptor.

  \param[in] fildes   File descriptor

  \return    Upon successful completion, 0 is returned.
             Otherwise, the function returns -1 and sets errno to indicate the error.
*/
__attribute__((weak))
int _close (int fildes) {
#if (defined(RTE_Compiler_IO_File) && defined(RTE_Compiler_IO_File_Interface))
  int32_t rval;
#endif

  switch (fildes) {
    case STDIN_FILENO:
      return (0);
    case STDOUT_FILENO:
      return (0);
    case STDERR_FILENO:
      return (0);
  }

#if defined(RTE_Compiler_IO_File)
#if defined(RTE_Compiler_IO_File_Interface)
  rval = rt_fs_close(fildes);
  if (rval < 0) {
    errno = rval;
    rval = -1;
  }
  return (rval);
#elif defined(RTE_Compiler_IO_File_BKPT)
  __asm("BKPT 0");
  return (-1);
#endif
#else
  return (-1);
#endif
}


/**
  Write on a file.

  The write() function attempts to write nbyte bytes from the buffer pointed to
  by buf to the file associated with the open file descriptor, fildes.

  \param[in] fildes   File descriptor
  \param[in] buf      A pointer to data buffer
  \param[in] nbyte    Number of bytes to write

  \return    Upon successful completion, this function returns the number
             of bytes actually written to the file associated with fildes.
             Otherwise, the function returns -1 and sets errno to indicate the error.
*/
__attribute__((weak))
ssize_t _write (int fildes, const void *buf, size_t nbyte) {
#if (defined(RTE_Compiler_IO_STDOUT) || defined(RTE_Compiler_IO_STDERR))
  int ch;
  const uint8_t *u8buf;
#elif !(defined(RTE_Compiler_IO_File) && defined(RTE_Compiler_IO_File_Interface))
  (void)buf;
  (void)nbyte;
#endif
  ssize_t sz;

  switch (fildes) {
    case STDIN_FILENO:
      return (-1);
    case STDOUT_FILENO:
      sz = 0;
#ifdef RTE_Compiler_IO_STDOUT
      u8buf = (const uint8_t *)buf;
      while (sz < nbyte) {
        ch = *u8buf++;
#if (STDOUT_CR_LF != 0)
        if (ch == '\n') {
          if (stdout_putchar('\r') == -1) {
            break;
          }
        }
#endif
        if (stdout_putchar(ch) == -1) {
          break;
        }
        sz++;
      }
#endif
      return (sz);
    case STDERR_FILENO:
      sz = 0;
#ifdef RTE_Compiler_IO_STDERR
      u8buf = (const uint8_t *)buf;
      while (sz < nbyte) {
        ch = *u8buf++;
#if (STDERR_CR_LF != 0)
        if (ch == '\n') {
          if (stderr_putchar('\r') == -1) {
            break;
          }
        }
#endif
        if (stderr_putchar(ch) == -1) {
          break;
        }
        sz++;
      }
#endif
      return (sz);
  }

#if defined(RTE_Compiler_IO_File)
#if defined(RTE_Compiler_IO_File_Interface)
  sz = rt_fs_write(fildes, buf, nbyte);
  if (sz < 0) {
    errno = sz;
    sz = -1;
  }
  return (sz);
#elif defined(RTE_Compiler_IO_File_BKPT)
  __asm("BKPT 0");
  return (-1);
#endif
#else
  return (-1);
#endif
}


/**
  Read from a file.

  The read() function attempts to read nbyte bytes from the file associated with
  the open file descriptor, fildes, into the buffer pointed to by buf.

  \param[in]  fildes   File descriptor
  \param[out] buf      A pointer to data buffer
  \param[in]  nbyte    Number of bytes to read

  \return     Upon successful completion, function returns a non-negative integer
              indicating the number of bytes actually read.
              Otherwise, the function returns -1 and sets errno to indicate the error.
*/
__attribute__((weak))
ssize_t _read (int fildes, void *buf, size_t nbyte) {
#ifdef RTE_Compiler_IO_STDIN
  int ch;
  uint8_t *u8buf;
#endif
#if (defined(RTE_Compiler_IO_File) && defined(RTE_Compiler_IO_File_Interface))
  ssize_t sz;
#else
  (void)buf;
  (void)nbyte;
#endif

  switch (fildes) {
    case STDIN_FILENO:
#ifdef RTE_Compiler_IO_STDIN
      u8buf = (uint8_t *)buf;
      ch = stdin_getchar();
      if (ch < 0) {
        return (0);
      }
      *u8buf++ = (uint8_t)ch;
#if (STDIN_ECHO != 0)
      stdout_putchar(ch);
#endif
      return (1);
#else
      return (0);
#endif
    case STDOUT_FILENO:
      return (-1);
    case STDERR_FILENO:
      return (-1);
  }

#if defined(RTE_Compiler_IO_File)
#if defined(RTE_Compiler_IO_File_Interface)
  sz = rt_fs_read(fildes, buf, nbyte);
  if (sz < 0) {
    errno = sz;
    sz = -1;
  }
  return (sz);
#elif defined(RTE_Compiler_IO_File_BKPT)
  __asm("BKPT 0");
  return (-1);
#endif
#else
  return (-1);
#endif
}


/**
  Test for a terminal device.

  The isatty() function tests whether fildes, an open file descriptor,
  is associated with a terminal device.

  \param[in] fildes File descriptor

  \return    The return value is 1 if fildes is associated with a terminal.
             Otherwise, 0 is returned.
*/
__attribute__((weak))
int _isatty (int fildes) {

  switch (fildes) {
    case STDIN_FILENO:
      return (1);
    case STDOUT_FILENO:
      return (1);
    case STDERR_FILENO:
      return (1);
  }

  return (0);
}


/**
  Move the read/write file offset.

  The lseek() function sets the file offset for the open file description associated
  with the file descriptor fildes, as follows:
  - If whence is SEEK_SET, the file offset is set to offset bytes.
  - If whence is SEEK_CUR, the file offset is set to its current location plus offset.
  - If whence is SEEK_END, the file offset is set to the size of the file plus offset.
  The file offset remains unchanged if the function fails.

  \param[in] fildes     File descriptor
  \param[in] offset     File offset
  \param[in] whence     File seek mode

  \return    Upon successful completion, the resulting offset in bytes from the beginning
             of the file is returned.
             Otherwise, the function returns -1 and sets errno to indicate the error.
*/
__attribute__((weak))
off_t _lseek (int fildes, off_t offset, int whence) {
#if (defined(RTE_Compiler_IO_File) && defined(RTE_Compiler_IO_File_Interface))
  int64_t rval;
#else
  (void)offset;
  (void)whence;
#endif

  switch (fildes) {
    case STDIN_FILENO:
      return (-1);
    case STDOUT_FILENO:
      return (-1);
    case STDERR_FILENO:
      return (-1);
  }

#if defined(RTE_Compiler_IO_File)
#if defined(RTE_Compiler_IO_File_Interface)
  rval = rt_fs_seek(fildes, offset, whence);
  if (rval < 0) {
    errno = (int)rval;
    rval = -1;
  }
  else {
    if ((sizeof(off_t) != sizeof(int64_t)) && ((rval >> 32) != 0)) {
      /* Returned file offset does not fit into off_t */
      rval = -1;
      errno = EOVERFLOW;
    }
  }
  return ((off_t)rval);
#elif defined(RTE_Compiler_IO_File_BKPT)
  __asm("BKPT 0");
  return (-1);
#endif
#else
  return (-1);
#endif
}


/**
  Get file status.

  The fstat() function obtains information about an open file associated with
  the file descriptor fildes, and writes it to the area pointed to by buf.

  \param[in]  fildes     File descriptor
  \param[out] buf        A pointer to a stat structure

  \return    Upon successful completion, 0 is returned.
             Otherwise, the function returns -1 and sets errno to indicate the error.
*/
__attribute__((weak))
int _fstat (int fildes, struct stat *buf) {
#if (defined(RTE_Compiler_IO_File) && defined(RTE_Compiler_IO_File_Interface))
  int32_t rval;
  rt_fs_stat_t rt_stat;
#endif

  /* Character-oriented device file for stdio streams */
  switch (fildes) {
    case STDIN_FILENO:
      buf->st_mode = S_IFCHR;
      return (0);
    case STDOUT_FILENO:
      buf->st_mode = S_IFCHR;
      return (0);
    case STDERR_FILENO:
      buf->st_mode = S_IFCHR;
      return (0);
  }

#if defined(RTE_Compiler_IO_File)
#if defined(RTE_Compiler_IO_File_Interface)
  rval = rt_fs_stat(fildes, &rt_stat);
  if (rval < 0) {
    errno = rval;
    rval = -1;
  } else {
    /* Convert "rt_fs_stat_t" content into "struct stat" format */
    /* Mode of file */
    if (rt_stat.attr & RT_ATTR_FILE) {
      buf->st_mode = S_IFREG;
    } else if (rt_stat.attr & RT_ATTR_DIR) {
      buf->st_mode = S_IFDIR;
    } else {
      buf->st_mode = S_IFLNK;
    }
    /* Timestamps */
    rt_to_stat_tim(&rt_stat.access, &buf->st_atim);
    rt_to_stat_tim(&rt_stat.modify, &buf->st_mtim);
    rt_to_stat_tim(&rt_stat.change, &buf->st_ctim);
    /* File system block size and count */
    buf->st_blksize = rt_stat.blksize;
    buf->st_blocks  = rt_stat.blkcount;
    /* File size */
    buf->st_size = rt_fs_size(fildes);
  }
  return (rval);
#elif defined(RTE_Compiler_IO_File_BKPT)
  __asm("BKPT 0");
  return (-1);
#endif
#else
  return (-1);
#endif
}

#if (defined(RTE_Compiler_IO_File) && defined(RTE_Compiler_IO_File_Interface))
/* Convert time from rt_fs_time_t to struct timespec format */
void rt_to_stat_tim(rt_fs_time_t *rt_t, struct timespec *st_t) {
  struct tm time_tm = {
    .tm_sec  = rt_t->sec,
    .tm_min  = rt_t->min,
    .tm_hour = rt_t->hour,
    .tm_mday = rt_t->day,
    .tm_mon  = rt_t->mon,
    .tm_year = rt_t->year - 1900 /* struct tm uses year since 1900 */
  };
  /* Conversion to seconds since Jan 1, 1970 */
  st_t->tv_sec  = mktime(&time_tm);
  st_t->tv_nsec = 0;
}
#endif

/**
  Get file status (by name).

  \param[in]  path       File name
  \param[in]  buf        A pointer to a stat structure

  \return    Upon successful completion, 0 is returned.
             Otherwise, the function returns -1 and sets errno to indicate the error.
*/
#ifdef RTE_Compiler_IO_File
__attribute__((weak))
int _stat (const char *path, struct stat *buf) {
  (void)path;
  (void)buf;
#if defined(RTE_Compiler_IO_File_BKPT)
  __asm("BKPT 0");
#endif
  /* Not implemented */
  errno = ENOSYS;
  return (-1);
}
#endif

/**
  Link one file to another file.

  \param[in]  path1       Pathname naming an existing file
  \param[out] path2       Pathname naming the new directory entry to be created

  \return    Upon successful completion, 0 is returned.
             Otherwise, the function returns -1 and sets errno to indicate the error.
*/
#ifdef RTE_Compiler_IO_File
__attribute__((weak))
int _link (const char *path1, const char *path2) {
#if defined(RTE_Compiler_IO_File_Interface)
  int32_t rval;
#else
  (void)path1;
  (void)path2;
#endif
#if defined(RTE_Compiler_IO_File_Interface)
  rval = rt_fs_rename(path1, path2);
  if (rval < 0) {
    errno = rval;
    rval = -1;
  }
  return (rval);
#elif defined(RTE_Compiler_IO_File_BKPT)
  __asm("BKPT 0");
  errno = ENOSYS;
  return (-1);
#endif
}
#endif


/**
  Remove a directory entry.

  \param[in]  path       File name

  \return    Upon successful completion, 0 is returned.
             Otherwise, the function returns -1 and sets errno to indicate the error.
*/
#ifdef RTE_Compiler_IO_File
__attribute__((weak))
int _unlink (const char *path) {
#if defined(RTE_Compiler_IO_File_Interface)
  int32_t rval;
#else
  (void)path;
#endif
#if defined(RTE_Compiler_IO_File_Interface)
  rval = rt_fs_remove (path);
  if (rval < 0) {
    errno = rval;
    rval = -1;
  }
  return (rval);
#elif defined(RTE_Compiler_IO_File_BKPT)
  __asm("BKPT 0");
  errno = ENOSYS;
  return (-1);
#endif
}
#endif


/* Empty list as a minimal implementation of the environment */
__attribute__((weak))
char *__env[1] = { 0 };

/* Environment points to an empty list */
__attribute__((weak))
char **environ = __env;


/* Execute a file */
__attribute__((weak))
int _execve(const char *path, char *const argv[], char *const envp[]) {
  (void)path;
  (void)argv;
  (void)envp;
  /* Transfering control to a new process is not supported */
  errno = ENOSYS;
  return (-1);
}


/* Create a new process */
__attribute__((weak))
int _fork (void) {
  /* Creating a process is not supported */
  errno = ENOSYS;
  return (-1);
}


/* Terminate a process */
__attribute__((weak))
void _exit (int status) {
  for(;;){ ; }
}


/* Send a signal to a process or a group of processes */
__attribute__((weak))
int _kill (pid_t pid, int sig) {
  (void)pid;
  (void)sig;
  /* Nothing to do */
  errno = ENOSYS;
  return (-1);
}


/* Get the current process id */
__attribute__((weak))
pid_t _getpid (void) {
  /* Only one process exists */
  return (0);
}


/* Wait for a child process to stop or terminate */
__attribute__((weak))
pid_t wait (int *stat_loc) {
  (void)stat_loc;
  /* Only one process, cannot wait for other processes */
  errno = ENOSYS;
  return (-1);
}


/* Extend heap space by incr bytes (reentrant version) */
__attribute__((weak))
void *_sbrk_r (struct _reent *reent, ptrdiff_t incr) {
  extern char  __HeapLimit;
  extern char  __HeapStart __asm("end");
  static char *heap;
         char *heap_prev;
         char *sp;
         void *p;

  sp = (char *)__get_MSP();

  if (heap == NULL) {
    /* Initialize current heap memory address */
    heap = &__HeapStart;
  }

  heap_prev = heap;

  if ((heap + incr) > &__HeapLimit) {
    /* Out of heap memory */
    reent->_errno = ENOMEM;

    p = (void *)-1;
  }
  else if ((heap + incr) >= sp) {
    /* Heap and stack collision */
    reent->_errno = ENOMEM;

    p = (void *)-1;
  }
  else {
    /* All good, extend heap space */
    heap += incr;

    p = (void *)heap_prev;
  }

  return (p);
}
