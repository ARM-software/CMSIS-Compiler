#ifndef USART_MPS2_H_
#define USART_MPS2_H_

#include <stdint.h>

#include "Driver_USART.h"
#include "CMSDK_UART.h"

#include "RTE_Components.h"
#include CMSIS_device_header

// USART Flags
#define USART_FLAG_INITIALIZED      ((uint8_t)(1U))
#define USART_FLAG_POWERED          ((uint8_t)(1U << 1))
#define USART_FLAG_CONFIGURED       ((uint8_t)(1U << 2))
#define USART_FLAG_TX_ENABLED       ((uint8_t)(1U << 3))
#define USART_FLAG_RX_ENABLED       ((uint8_t)(1U << 4))

// USART Transfer Information (Run-Time)
typedef struct _USART_TRANSFER_INFO {
  uint32_t              rx_num;         // Total number of receive data
  uint32_t              tx_num;         // Total number of transmit data
  uint8_t              *rx_buf;         // Pointer to in data buffer
  const uint8_t        *tx_buf;         // Pointer to out data buffer
  uint32_t              rx_cnt;         // Number of data received
  uint32_t              tx_cnt;         // Number of data sent
} USART_TRANSFER_INFO;

typedef struct _USART_STATUS {
  uint8_t tx_busy;                      // Transmitter busy flag
  uint8_t rx_busy;                      // Receiver busy flag
  uint8_t rx_overflow;                  // Receive data overflow detected (cleared on start of next receive operation)
} USART_STATUS;

// USART Information (Run-Time)
typedef struct _USART_INFO {
  ARM_USART_SignalEvent_t cb_event;     // Event callback
  USART_STATUS            status;       // Status flags
  uint8_t                 flags;        // Current USART flags
} USART_INFO;

// USART Resources Definitions
typedef const struct {
  ARM_USART_CAPABILITIES  capabilities; // Capabilities
  CMSDK_UART_TypeDef      *reg;         // USART peripheral pointer
  IRQn_Type                irq_num_rx;  // USART RX IRQ Number
  IRQn_Type                irq_num_tx;  // USART TX IRQ Number
  USART_INFO              *info;        // Run-Time Information
  USART_TRANSFER_INFO     *xfer;        // USART transfer information
} USART_RESOURCES;

#endif /* USART_MPS2_H_ */
