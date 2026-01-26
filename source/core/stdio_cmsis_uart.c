/*---------------------------------------------------------------------------
 * Copyright (c) 2026 Arm Limited (or its affiliates).
 * All rights reserved.
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
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include "cmsis_compiler.h"

#include "RTE_Components.h"

#ifdef RTE_CMSIS_Compiler_STDERR_UART_CMSIS
#include "retarget_stderr.h"
#include "stderr_cmsis_uart_config.h"
#endif
#ifdef RTE_CMSIS_Compiler_STDIN_UART_CMSIS
#include "retarget_stdin.h"
#include "stdin_cmsis_uart_config.h"
#endif
#ifdef RTE_CMSIS_Compiler_STDOUT_UART_CMSIS
#include "retarget_stdout.h"
#include "stdout_cmsis_uart_config.h"
#endif

#include "stdio_cmsis_uart_config.h"

/* Global initialization function */
extern int stdio_cmsis_uart_init (void);

/* Reference to the underlying USART driver */
#define ptrUSART  (&ARM_Driver_USART_(STDIO_UART_DRIVER))

/* Global Initialization Flag */
static uint8_t Initialized = 0U;

/* UART Error Flags */
#define UART_FLAG_RX_ERROR (0x01U)
#define UART_FLAG_TX_ERROR (0x02U)

/* UART Input Buffer Structure */
typedef struct {
           uint8_t *data;   /* Pointer to data buffer           */
           uint32_t size;   /* Size of data buffer              */
  volatile uint32_t cnt;    /* Number of characters transferred */
  volatile uint32_t idx;    /* Buffer index                     */
  volatile uint32_t flags;  /* Error flags                      */
} UART_InBuffer;

/* UART Output Buffer Structure */
typedef struct {
           uint8_t *data;   /* Pointer to data buffer           */
           uint32_t size;   /* Size of data buffer              */
  volatile uint32_t idx;    /* Buffer index                     */
  volatile uint32_t flags;  /* Error flags                      */
} UART_OutBuffer;

#if defined(RTE_CMSIS_Compiler_STDERR_UART_CMSIS)
/* STDERR Buffer Attribute */
#define STDERR_UART_BUFFER_ATTRIBUTE __attribute__((section(STDERR_UART_BUFFER_SECTION), aligned(STDERR_UART_BUFFER_ALIGNMENT)))
/* STDERR Buffer Array */
static uint8_t StdErrBuf[(STDERR_UART_BUFFER_SIZE == 0) ? 1 : STDERR_UART_BUFFER_SIZE] STDERR_UART_BUFFER_ATTRIBUTE;
/* STDERR Buffer Object */
static UART_OutBuffer UART_StdErrBuf = { StdErrBuf, sizeof(StdErrBuf), 0, 0 };
#endif

#if defined(RTE_CMSIS_Compiler_STDIN_UART_CMSIS)
/* STDIN Buffer Attribute */
#define STDIN_UART_BUFFER_ATTRIBUTE __attribute__((section(STDIN_UART_BUFFER_SECTION), aligned(STDIN_UART_BUFFER_ALIGNMENT)))
/* STDIN Buffer Array */
static uint8_t StdInBuf[(STDIN_UART_BUFFER_SIZE == 0) ? 1 : STDIN_UART_BUFFER_SIZE] STDIN_UART_BUFFER_ATTRIBUTE;
/* STDIN Buffer Object */
static UART_InBuffer UART_StdInBuf = { StdInBuf, sizeof(StdInBuf), 0, 0, 0 };
#endif

#if defined(RTE_CMSIS_Compiler_STDOUT_UART_CMSIS)
/* STDOUT Buffer Attribute */
#define STDOUT_UART_BUFFER_ATTRIBUTE __attribute__((section(STDOUT_UART_BUFFER_SECTION), aligned(STDOUT_UART_BUFFER_ALIGNMENT)))
/* STDOUT Buffer Array */
static uint8_t StdOutBuf[(STDOUT_UART_BUFFER_SIZE == 0) ? 1 : STDOUT_UART_BUFFER_SIZE] STDOUT_UART_BUFFER_ATTRIBUTE;
/* STDOUT Buffer Object */
static UART_OutBuffer UART_StdOutBuf = { StdOutBuf, sizeof(StdOutBuf), 0, 0 };
#endif

/**
  UART driver event callback

  \param event  ARM_USART_EVENT_* notification mask
 */
void UART_Event_Callback(uint32_t event) {
#if defined(RTE_CMSIS_Compiler_STDIN_UART_CMSIS)
  int32_t status;

  if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
    /* Increment number of received characters */
    UART_StdInBuf.cnt += UART_StdInBuf.size;

    /* Initiate new receive */
    status = ptrUSART->Receive(&UART_StdInBuf.data[0], UART_StdInBuf.size);

    if (status != ARM_DRIVER_OK) {
      /* Set error flag */
      UART_StdInBuf.flags |= UART_FLAG_RX_ERROR;
    }
  }
#endif
}

/*
  Initialize stdio
*/
__WEAK
int stdio_init (void) {
  return stdio_cmsis_uart_init();
}

/**
  Initialize stdio retarget to CMSIS-Driver UART

  \return          0 on success, or -1 on error.
*/
int stdio_cmsis_uart_init (void) {
  int rval;

  if (Initialized != 0U) {
    /* Already initialized */
    return 0;
  }

  if (ptrUSART->Initialize(UART_Event_Callback) != ARM_DRIVER_OK) {
    /* CMSIS-Driver USART initialize failed */
    rval = -1;
  }
  else if (ptrUSART->PowerControl(ARM_POWER_FULL) != ARM_DRIVER_OK) {
    /* CMSIS-Driver USART power full failed */
    rval = -2;
  }
  else if (ptrUSART->Control(ARM_USART_MODE_ASYNCHRONOUS |
                             ARM_USART_DATA_BITS_8       |
                             ARM_USART_PARITY_NONE       |
                             ARM_USART_STOP_BITS_1       |
                             ARM_USART_FLOW_CONTROL_NONE,
                             STDIO_UART_BAUDRATE) != ARM_DRIVER_OK) {
    /* CMSIS-Driver USART mode configuration failed */
    rval = -3;
  }
#if defined(RTE_CMSIS_Compiler_STDERR_UART_CMSIS) || defined(RTE_CMSIS_Compiler_STDOUT_UART_CMSIS)
  else if (ptrUSART->Control(ARM_USART_CONTROL_TX, 1U) != ARM_DRIVER_OK) {
    /* CMSIS-Driver USART transmitter enable failed */
    rval = -4;
  }
#endif
#if defined(RTE_CMSIS_Compiler_STDIN_UART_CMSIS)
  else if (ptrUSART->Control(ARM_USART_CONTROL_RX, 1U) != ARM_DRIVER_OK) {
    /* CMSIS-Driver USART receiver enable failed */
    rval = -5;
  }
  else if (ptrUSART->Receive(&UART_StdInBuf.data[0], UART_StdInBuf.size) != ARM_DRIVER_OK) {
    /* CMSIS-Driver USART receive operation failed */
    rval = -6;
  }
#endif
  else {
    /* CMSIS-Driver USART setup completed successfully */
    rval = 0;

    /* Set global initialized flag */
    Initialized = 1U;
  }

  return rval;
}

#if defined(RTE_CMSIS_Compiler_STDERR_UART_CMSIS)
/**
  Put a character to the stderr

  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int stderr_putchar (int ch) {
  int32_t  status;

  if (Initialized == 0U) {
    /* Initialize stdio if not already initialized */
    if (stdio_cmsis_uart_init() != 0) {
      return -1;
    }
  }

  /* Write character at current index */
  UART_StdErrBuf.data[UART_StdErrBuf.idx] = (uint8_t)ch;

  /* Increment number of characters written */
  UART_StdErrBuf.idx++;

  /* Send when the buffer is full or on newline character */
  if ((UART_StdErrBuf.idx == UART_StdErrBuf.size) || (ch == '\n')) {
    /* Send buffered characters */
    status = ptrUSART->Send(&UART_StdErrBuf.data[0], UART_StdErrBuf.idx);

    if (status != ARM_DRIVER_OK) {
      /* Set error flag */
      UART_StdErrBuf.flags |= UART_FLAG_TX_ERROR;
    }

    /* Wait for send to complete */
    while (ptrUSART->GetStatus().tx_busy != 0U);

    /* Reset transmit index */
    UART_StdErrBuf.idx = 0U;
  }

  return ch;
}
#endif

#if defined(RTE_CMSIS_Compiler_STDIN_UART_CMSIS)
/**
  Get a character from the stdio

  \return     The next character from the input, or -1 on read error.
*/
int stdin_getchar (void) {
  uint32_t rx_num;
  uint32_t rx_cnt;
  uint32_t rx_idx;
  int ch = -1;

  if (Initialized == 0U) {
    /* Initialize stdio if not already initialized */
    if (stdio_cmsis_uart_init() != 0) {
      return -1;
    }
  }

  while (ch == -1) {
    /* Get the number of received characters since last Receive operation */
    rx_num = ptrUSART->GetRxCount();

    /* Determine total number of received characters */
    rx_cnt = UART_StdInBuf.cnt + rx_num;

    if (UART_StdInBuf.idx < rx_cnt) {
      /* There are characters available to read */
      rx_idx = UART_StdInBuf.idx % UART_StdInBuf.size;
      /* Read character at current index */
      ch = UART_StdInBuf.data[rx_idx];

      /* Increment number of characters read */
      UART_StdInBuf.idx++;
    }
  }

  return ch;
}
#endif

#if defined(RTE_CMSIS_Compiler_STDOUT_UART_CMSIS)
/**
  Put a character to the stdout

  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int stdout_putchar (int ch) {
  int32_t  status;

  if (Initialized == 0U) {
    /* Initialize stdio if not already initialized */
    if (stdio_cmsis_uart_init() != 0) {
      return -1;
    }
  }

  /* Write character at current index */
  UART_StdOutBuf.data[UART_StdOutBuf.idx] = (uint8_t)ch;

  /* Increment number of characters written */
  UART_StdOutBuf.idx++;

  /* Send when the buffer is full or on newline character */
  if ((UART_StdOutBuf.idx == UART_StdOutBuf.size) || (ch == '\n')) {
    /* Send buffered characters */
    status = ptrUSART->Send(&UART_StdOutBuf.data[0], UART_StdOutBuf.idx);

    if (status != ARM_DRIVER_OK) {
      /* Set error flag */
      UART_StdOutBuf.flags |= UART_FLAG_TX_ERROR;
    }

    /* Wait for send to complete */
    while (ptrUSART->GetStatus().tx_busy != 0U);

    /* Reset transmit index */
    UART_StdOutBuf.idx = 0U;
  }

  return ch;
}
#endif
