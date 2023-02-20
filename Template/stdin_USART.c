/*-----------------------------------------------------------------------------
 * Name:    stdin_USART.c
 * Purpose: STDIN USART Template
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
 
#include "Driver_USART.h"
 
//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
 
// <h>STDIN USART Interface
 
//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for USART interface
#define USART_DRV_NUM           0
 
//   <o>Baudrate
#define USART_BAUDRATE          9600
 
// </h>
 
 
#define _USART_Driver_(n)  Driver_USART##n
#define  USART_Driver_(n) _USART_Driver_(n)
 
extern ARM_DRIVER_USART  USART_Driver_(USART_DRV_NUM);
#define ptrUSART       (&USART_Driver_(USART_DRV_NUM))
 
 
/**
  Initialize stdin
 
  \return          0 on success, or -1 on error.
*/
int stdin_init (void) {
  int32_t status;
 
  status = ptrUSART->Initialize(NULL);
  if (status != ARM_DRIVER_OK) return (-1);
 
  status = ptrUSART->PowerControl(ARM_POWER_FULL);
  if (status != ARM_DRIVER_OK) return (-1);
 
  status = ptrUSART->Control(ARM_USART_MODE_ASYNCHRONOUS |
                             ARM_USART_DATA_BITS_8       |
                             ARM_USART_PARITY_NONE       |
                             ARM_USART_STOP_BITS_1       |
                             ARM_USART_FLOW_CONTROL_NONE,
                             USART_BAUDRATE);
  if (status != ARM_DRIVER_OK) return (-1);
 
  status = ptrUSART->Control(ARM_USART_CONTROL_RX, 1);
  if (status != ARM_DRIVER_OK) return (-1);
 
  return (0);
}
 
 
/**
  Get a character from stdin
 
  \return     The next character from the input, or -1 on read error.
*/
int stdin_getchar (void) {
  uint8_t buf[1];
 
  if (ptrUSART->Receive(buf, 1) != ARM_DRIVER_OK) {
    return (-1);
  }
  while (ptrUSART->GetRxCount() != 1);
  return (buf[0]);
}
