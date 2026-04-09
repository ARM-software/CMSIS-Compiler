/*---------------------------------------------------------------------------
 * Copyright (c) 2025 Arm Limited (or its affiliates).
 * All rights reserved.
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

#ifndef MPS2_MEMMAP_H
#define MPS2_MEMMAP_H

/*============================================================================*/
/*                              MEMORY REGIONS                                */
/*============================================================================*/

/*---------------------------------------------------------------------------*/
/*                              Code Regions                                 */
/*---------------------------------------------------------------------------*/

/* ZBTSRAM 1 (Code/Startup) */
#define ZBTSRAM1_BASE                   0x00000000
#define ZBTSRAM1_SIZE                   0x00400000    /* 4MB */
#define ZBTSRAM1_LIMIT                  (ZBTSRAM1_BASE + ZBTSRAM1_SIZE - 1)

/*---------------------------------------------------------------------------*/
/*                               SRAM Regions                                */
/*---------------------------------------------------------------------------*/

/* ZBTSRAM 2 and 3 (Data) */
#define ZBTSRAM23_BASE                  0x20000000
#define ZBTSRAM23_SIZE                  0x00400000    /* 4MB */
#define ZBTSRAM23_LIMIT                 (ZBTSRAM23_BASE + ZBTSRAM23_SIZE - 1)

/* PSRAM */
#define PSRAM_BASE                      0x60000000
#define PSRAM_SIZE                      0x01000000    /* 16MB */
#define PSRAM_LIMIT                     (PSRAM_BASE + PSRAM_SIZE - 1)

#endif /* MPS2_MEMMAP_H */
