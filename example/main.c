/*---------------------------------------------------------------------------
 * Copyright (c) 2023 Arm Limited (or its affiliates). All rights reserved.
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

#include "RTE_Components.h"

#include  CMSIS_device_header

#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include "retarget_stdout.h"

int main (void) {
  stdout_init();

#if defined(RTE_Compiler_EventRecorder) && \
    (defined(__MICROLIB) || \
    !(defined(RTE_CMSIS_RTOS2_RTX5) || defined(RTE_CMSIS_RTOS2_FreeRTOS)))
  EventRecorderInitialize(EventRecordAll, 1U);
  EventRecorderClockUpdate();
#endif

  printf("Starting ...\r\n");

  int32_t count = 0;

  while (1)  {
    printf ("Hello World %d\r\n", count);

    if (count > 100) break;
    count++;

    for (int32_t i=10000; i>0; i--) __NOP();
  }

  printf("\r\nFinished\r\n\x04");

  return 0;
}

void HardFault_Handler(void) {
  printf("\r\n=== PANIC ===\r\n\x04");
  exit(1);
}
