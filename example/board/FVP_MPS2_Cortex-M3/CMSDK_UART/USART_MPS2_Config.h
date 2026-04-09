#ifndef USART_MPS2_CONFIG_H_
#define USART_MPS2_CONFIG_H_

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
//------ With VS Code: Open Preview for Configuration Wizard -------------------

// <o> CMSDK UART Base Address <0x0-0xFFFFFFFF:8>
// <i> Defines the CMSDK UART peripheral base address
#define CMSDK_UART_BASE         0x40004000

// <h> UART RX Interrupt
//   <o> Interrupt Number (IRQn)
//   <i> The interrupt number (IRQn) for UART RX interrupt
#define UART0RX_IRQn            0

//   <y> Interrupt Handler
//   <i> The name of the UART RX interrupt handler
//   <i> Default: Interrupt0_Handler
#define UART0RX_Handler         Interrupt0_Handler
// </h>

// <h> UART TX Interrupt
//   <o> Interrupt Number (IRQn)
//   <i> The interrupt number (IRQn) for UART TX interrupt
#define UART0TX_IRQn             1

//   <y> Interrupt Handler
//   <i> The name of the UART TX interrupt handler
//   <i> Default: Interrupt1_Handler
#define UART0TX_Handler         Interrupt1_Handler
// </h>

#endif /* USART_MPS2_CONFIG_H_ */
