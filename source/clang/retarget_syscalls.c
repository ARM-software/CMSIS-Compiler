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

#include <stdio.h>

#include "RTE_Components.h"

#ifdef RTE_Compiler_IO_STDOUT_EVR
#include "EventRecorder.h"
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

static int stdin_getc(FILE *file) {
    (void)file;
    return stdin_getchar();
}
static FILE __stdin = FDEV_SETUP_STREAM(NULL, _getc, NULL, _FDEV_SETUP_READ);
FILE *const stdin = &__stdio;

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

static int stdout_putc(char c, FILE *file) {
    (void)file;
    return stdout_putchar(c);
}
static FILE __stdout = FDEV_SETUP_STREAM(stdout_putc, NULL, NULL, _FDEV_SETUP_WRITE);
FILE *const stdout = &__stdout;

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

static int stderr_putc(char c, FILE *file) {
    (void)file;
    return stderr_putchar(c);
}
static FILE __stderr = FDEV_SETUP_STREAM(stderr_putc, NULL, NULL, _FDEV_SETUP_WRITE);
FILE *const stderr = &__stderr;
#endif

/* Terminate a process */
__attribute__((weak, __noreturn__))
void _exit (int status) {
  for(;;){ ; }
}
