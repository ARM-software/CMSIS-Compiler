#ifndef REGIONS_MPS2_H
#define REGIONS_MPS2_H

#include "mps2_memmap.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <h> Image Memory Configuration

// <h> ROM
// =========
// <h> __ROM0
//   <y> Base address
//   <i> Defines base address of memory region.
//   <i> Contains Startup and Vector Table
//   <i> Default: ZBTSRAM1_BASE
#define __ROM0_BASE   0x00000000
//   <y> Region size [bytes]
//   <i> Defines size of memory region.
//   <i> Default: ZBTSRAM1_SIZE
#define __ROM0_SIZE   0x003F8000
// </h>

// </h>

// <h> RAM
// =========
// <h> __RAM0
//   <y> Base address
//   <i> Defines base address of memory region.
//   <i> Default: ZBTSRAM23_BASE
#define __RAM0_BASE   0x20008000
//   <y> Region size [bytes]
//   <i> Defines size of memory region.
//   <i> Default: ZBTSRAM23_SIZE
#define __RAM0_SIZE   0x00040000
// </h>

// </h>

// <h> Stack / Heap
// ==================
//   <o0> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
//   <o1> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
#define __STACK_SIZE  0x00000800
#define __HEAP_SIZE   0x00001000
// </h>

// </h>

#endif /* REGIONS_MPS2_H */
