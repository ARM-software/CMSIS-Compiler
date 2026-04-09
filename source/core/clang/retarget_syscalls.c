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

#include <stdio.h>

#include "RTE_Components.h"

#ifdef RTE_CMSIS_Compiler_STDERR
#include "retarget_stderr.h"
#ifdef RTE_CMSIS_Compiler_STDERR_UART_CMSIS
#include "stderr_cmsis_uart_config.h"
#endif
#endif

#ifdef RTE_CMSIS_Compiler_STDIN
#include "retarget_stdin.h"
#ifdef RTE_CMSIS_Compiler_STDIN_UART_CMSIS
#include "stdin_cmsis_uart_config.h"
#endif
#endif

#ifdef RTE_CMSIS_Compiler_STDOUT
#include "retarget_stdout.h"
#ifdef RTE_CMSIS_Compiler_STDOUT_UART_CMSIS
#include "stdout_cmsis_uart_config.h"
#endif
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

#if defined(__LLVM_LIBC__)

/* Symbols used as cookie values to identify standard I/O stream */
void *__llvm_libc_stdin_cookie  = NULL;
void *__llvm_libc_stdout_cookie = NULL;
void *__llvm_libc_stderr_cookie = NULL;

/**
  Write bytes to a standard output stream.

  Called by LLVM-libc for all stdout/stderr writes.  The cookie parameter
  identifies the target stream: it matches the address of the
  __llvm_libc_stdout_cookie or __llvm_libc_stderr_cookie symbol.

  \param[in]  cookie  Stream identifier
  \param[in]  buf     Pointer to data buffer
  \param[in]  size    Number of bytes to write
  \return             Number of bytes written, or -1 on error.
*/
__attribute__((weak))
ssize_t __llvm_libc_stdio_write(void *cookie, const char *buf, size_t size) {
#if (defined(RTE_CMSIS_Compiler_STDOUT) || defined(RTE_CMSIS_Compiler_STDERR))
  int ch;
  ssize_t sz = 0;
  const char *p = buf;
#else
  (void)cookie;
  (void)buf;
  (void)size;
#endif

#if defined(RTE_CMSIS_Compiler_STDOUT)
  if (cookie == &__llvm_libc_stdout_cookie) {
    while (sz < (ssize_t)size) {
      ch = *p++;
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
    return (sz);
  }
#endif

#if defined(RTE_CMSIS_Compiler_STDERR)
  if (cookie == &__llvm_libc_stderr_cookie) {
    while (sz < (ssize_t)size) {
      ch = *p++;
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
    return (sz);
  }
#endif
  /* Not implemented */
  return (-1);
}

/**
  Read bytes from stdin.

  Called by LLVM-libc for all stdin reads.  The cookie parameter matches the
  address of the __llvm_libc_stdin_cookie symbol.

  \param[in]  cookie  Stream identifier
  \param[out] buf     Pointer to data buffer
  \param[in]  size    Maximum number of bytes to read
  \return             Number of bytes read (> 0), 0 on end of input,
                      or a negative errno value on error.
*/
__attribute__((weak))
ssize_t __llvm_libc_stdio_read(void *cookie, char *buf, size_t size) {
#if defined(RTE_CMSIS_Compiler_STDIN)
  int ch;
  ssize_t sz = 0;
  char *p = buf;
#else
  (void)cookie;
  (void)buf;
  (void)size;
#endif

#if defined(RTE_CMSIS_Compiler_STDIN)
  if (cookie == &__llvm_libc_stdin_cookie) {
    while (sz < (ssize_t)size) {
      ch = stdin_getchar();
      if (ch < 0) {
        break;
      }
      *p++ = (char)ch;
#if (STDIN_ECHO != 0)
      stdout_putchar(ch);
#endif
      sz++;
    }
    return sz;
  }
#endif
  /* Not implemented */
  return (-1);
}

/**
  Terminate a process.

  Called by LLVM-libc for all process termination. 
  The status parameter is the exit code passed to the exit() function. This
  implementation does not support process termination, so it simply enters
  an infinite loop.

  \param[in]  status  Exit code passed to exit()
*/
__attribute__((weak, __noreturn__))
void __llvm_libc_exit(int status) {
  (void)status;
  for (;;) { ; }
}

/**
  Set up the exceptions table and enable relevant interrupts.

  This function overrides crt0's _platform_setup_exceptions which sets VTOR
  to its own minimal 16-entry vector table, breaking device IRQ handlers.
  Restoring VTOR to the full CMSIS vector table (256 entries) ensures device-
  specific interrupt vectors are resolved correctly.
*/
__attribute__((weak))
void _platform_setup_exceptions (void) {}

/**
  Set up the Memory Management Unit and caches.

  This function overrides crt0's _platform_setup_memory which may override settings
  applied in vendor specific SystemInit.
*/
__attribute__((weak))
void _platform_setup_memory (void) {}

/**
  Set up architecture extensions that require special initialization.

  This function overrides crt0's _platform_setup_arch_extensions which may override
  settings applied in vendor specific SystemInit.
*/
__attribute__((weak))
void _platform_setup_arch_extensions (void) {}

#elif defined(__PICOLIBC_VERSION__)

#if defined(RTE_CMSIS_Compiler_STDIN)
/**
  Get a character from the stdio

  \return     The next character from the input, or -1 on read error.
*/
static int stdin_getc(FILE *file) {
  int ch;
  (void)file;

  ch = stdin_getchar();
#if (STDIN_ECHO != 0)
  if (ch >= 0) {
    stdout_putchar(ch);
  }
#endif
  return (ch);
}

static FILE __stdin = FDEV_SETUP_STREAM(NULL,
                                        stdin_getc,
                                        NULL,
                                        _FDEV_SETUP_READ);
FILE *const stdin = &__stdin;

#endif

#if defined(RTE_CMSIS_Compiler_STDOUT)
/**
  Put a character to the stdout

  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
static int stdout_putc(char ch, FILE *file) {
  (void)file;

#if (STDOUT_CR_LF != 0)
  if (ch == '\n') {
    stdout_putchar('\r');
  }
#endif
  return stdout_putchar(ch);
}

static FILE __stdout = FDEV_SETUP_STREAM(stdout_putc,
                                         NULL,
                                         NULL,
                                         _FDEV_SETUP_WRITE);
FILE *const stdout = &__stdout;
#endif

#if defined(RTE_CMSIS_Compiler_STDERR)
/**
  Put a character to the stderr

  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
static int stderr_putc(char ch, FILE *file) {
  (void)file;

#if (STDERR_CR_LF != 0)
  if (ch == '\n') {
    stderr_putchar('\r');
  }
#endif

  return stderr_putchar(ch);
}

static FILE __stderr = FDEV_SETUP_STREAM(stderr_putc,
                                         NULL,
                                         NULL,
                                         _FDEV_SETUP_WRITE);
FILE *const stderr = &__stderr;
#endif

/* Terminate a process */
__attribute__((weak, __noreturn__))
void _exit (int status) {
  for(;;){ ; }
}
#endif /* __LLVM_LIBC__ */
