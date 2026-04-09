
#include <string.h>

#include "CMSDK_UART.h"

#include "USART_MPS2_Config.h"
#include "USART_MPS2.h"


#define ARM_USART_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)


#define CMSDK_UART0 ((CMSDK_UART_TypeDef *) CMSDK_UART_BASE)

// USART0 Run-Time Information
static USART_INFO          USART0_Info         = { 0U, { 0U, 0U, 0U }, 0U };
static USART_TRANSFER_INFO USART0_TransferInfo = { 0U, 0U, 0U, 0U, 0U, 0U };


// USART0 Resources
static const USART_RESOURCES USART0_Resources = {
  {     // Capabilities
    1,  // supports UART (Asynchronous) mode
    0,  // supports Synchronous Master mode
    0,  // supports Synchronous Slave mode
    0,  // supports UART Single-wire mode
    0,  // supports UART IrDA mode
    0,  // supports UART Smart Card mode
    0,  // Smart Card Clock generator
    0,  // RTS Flow Control available
    0,  // CTS Flow Control available
    1,  // Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE
    1,  // Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
    0,  // RTS Line: 0=not available, 1=available
    0,  // CTS Line: 0=not available, 1=available
    0,  // DTR Line: 0=not available, 1=available
    0,  // DSR Line: 0=not available, 1=available
    0,  // DCD Line: 0=not available, 1=available
    0,  // RI Line: 0=not available, 1=available
    0,  // Signal CTS change event: \ref ARM_USART_EVENT_CTS
    0,  // Signal DSR change event: \ref ARM_USART_EVENT_DSR
    0,  // Signal DCD change event: \ref ARM_USART_EVENT_DCD
    0,  // Signal RI change event: \ref ARM_USART_EVENT_RI
    0   // Reserved (must be zero)
  },
  CMSDK_UART0,
  (IRQn_Type)UART0RX_IRQn,
  (IRQn_Type)UART0TX_IRQn,
  &USART0_Info,
  &USART0_TransferInfo
};

/**
  \fn          ARM_DRIVER_VERSION USARTx_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION USARTx_GetVersion (void) {
  const ARM_DRIVER_VERSION usart_driver_version = {
    ARM_USART_API_VERSION,
    ARM_USART_DRV_VERSION
  };

  return usart_driver_version;
}

/**
  \fn          ARM_USART_CAPABILITIES USART_GetCapabilities (const USART_RESOURCES *usart)
  \brief       Get driver capabilities
  \param[in]   usart     Pointer to USART resources
  \return      \ref ARM_USART_CAPABILITIES
*/
static ARM_USART_CAPABILITIES USART_GetCapabilities (const USART_RESOURCES *usart) {
  return usart->capabilities;
}

/**
  \fn          int32_t USART_Initialize (      ARM_USART_SignalEvent_t  cb_event
                                         const USART_RESOURCES         *usart)
  \brief       Initialize USART Interface.
  \param[in]   cb_event  Pointer to \ref ARM_USART_SignalEvent
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Initialize (      ARM_USART_SignalEvent_t  cb_event,
                                 const USART_RESOURCES         *usart) {

  if (usart->info->flags & USART_FLAG_INITIALIZED) {
    // Driver is already initialized
    return ARM_DRIVER_OK;
  }

  // Initialize callback
  usart->info->cb_event = cb_event;

  // Clear transfer information
  memset(usart->xfer, 0U, sizeof(USART_TRANSFER_INFO));

  usart->info->flags = USART_FLAG_INITIALIZED;                  // Set status flags

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Uninitialize (const USART_RESOURCES *usart)
  \brief       De-initialize USART Interface.
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Uninitialize (const USART_RESOURCES *usart) {

  usart->info->flags = 0U;                                      // Reset status flags

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_PowerControl (ARM_POWER_STATE state)
  \brief       Control USART Interface Power.
  \param[in]   state  Power state
  \param[in]   usart  Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_PowerControl (      ARM_POWER_STATE  state,
                                   const USART_RESOURCES *usart) {

  switch (state) {
    case ARM_POWER_OFF:
      // Clear powered flag
      usart->info->flags &= ~USART_FLAG_POWERED;

      // Clear and disable USART IRQ
      NVIC_DisableIRQ(usart->irq_num_rx);
      NVIC_ClearPendingIRQ(usart->irq_num_rx);
      NVIC_DisableIRQ(usart->irq_num_tx);
      NVIC_ClearPendingIRQ(usart->irq_num_tx);
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if ((usart->info->flags & USART_FLAG_INITIALIZED) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((usart->info->flags & USART_FLAG_POWERED)     != 0U) {
        return ARM_DRIVER_OK;
      }

      // Initialize status flags
      usart->info->status.tx_busy       = 0U;
      usart->info->status.rx_busy       = 0U;
      usart->info->status.rx_overflow   = 0U;

      // Set flag initialized
      usart->info->flags = USART_FLAG_POWERED | USART_FLAG_INITIALIZED;

      // Clear and enable USART IRQ
      NVIC_ClearPendingIRQ(usart->irq_num_rx);
      NVIC_EnableIRQ(usart->irq_num_rx);
      NVIC_ClearPendingIRQ(usart->irq_num_tx);
      NVIC_EnableIRQ(usart->irq_num_tx);
      break;

//    default: return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Send (const void            *data,
                                         uint32_t         num,
                                   const USART_RESOURCES *usart)
  \brief       Start sending data to USART transmitter.
  \param[in]   data  Pointer to buffer with data to send to USART transmitter
  \param[in]   num   Number of data items to send
  \param[in]   usart Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Send (const void            *data,
                                 uint32_t         num,
                           const USART_RESOURCES *usart) {

  if ((data == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  if ((usart->info->status.tx_busy != 0U) ||
     ((usart->reg->STATE & CMSDK_UART_STATE_TXBF_Msk) != 0U)) {
    // Send is not completed yet
    return ARM_DRIVER_ERROR_BUSY;
  }

  // Set tx_busy flag
  usart->info->status.tx_busy = 1U;

  // Save transmit buffer info
  usart->xfer->tx_buf = (const uint8_t *)data;
  usart->xfer->tx_num = num;
  usart->xfer->tx_cnt = 0U;

  // TX interrupt enable
  usart->reg->CTRL |= CMSDK_UART_CTRL_TXIRQEN_Msk;

  // Start sending
  usart->reg->DATA = usart->xfer->tx_buf[usart->xfer->tx_cnt];

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Receive (      void            *data,
                                            uint32_t         num,
                                      const USART_RESOURCES *usart)
  \brief       Start receiving data from USART receiver.
  \param[out]  data  Pointer to buffer for data to receive from USART receiver
  \param[in]   num   Number of data items to receive
  \param[in]   usart Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Receive (      void            *data,
                                    uint32_t         num,
                              const USART_RESOURCES *usart) {

  if ((data == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  // Check if receiver is busy
  if ((usart->info->status.rx_busy != 0U) ||
     ((usart->reg->STATE & CMSDK_UART_STATE_RXBF_Msk) != 0U)) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  // Save number of data to be received
  usart->xfer->rx_num = num;

  // Clear RX status
  usart->info->status.rx_overflow = 0U;

  // Save receive buffer info
  usart->xfer->rx_buf = (uint8_t *)data;
  usart->xfer->rx_cnt =  0U;

  // Set RX busy flag
  usart->info->status.rx_busy = 1U;

  // RX interrupt enable
  usart->reg->CTRL |= CMSDK_UART_CTRL_RXIRQEN_Msk;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Transfer (const void             *data_out,
                                             void             *data_in,
                                             uint32_t          num,
                                       const USART_RESOURCES  *usart)
  \brief       Start sending/receiving data to/from USART transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to USART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from USART receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Transfer (const void             *data_out,
                                     void             *data_in,
                                     uint32_t          num,
                               const USART_RESOURCES  *usart) {
  (void)data_out;
  (void)data_in;
  (void)num;
  (void)usart;

  // Synchronous mode not supported
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn          uint32_t USART_GetTxCount (const USART_RESOURCES *usart)
  \brief       Get transmitted data count.
  \param[in]   usart     Pointer to USART resources
  \return      number of data items transmitted
*/
static uint32_t USART_GetTxCount (const USART_RESOURCES *usart) {

  return usart->xfer->tx_cnt;

}

/**
  \fn          uint32_t USART_GetRxCount (const USART_RESOURCES *usart)
  \brief       Get received data count.
  \param[in]   usart     Pointer to USART resources
  \return      number of data items received
*/
static uint32_t USART_GetRxCount (const USART_RESOURCES *usart) {

  return usart->xfer->rx_cnt;

}

/**
  \fn          int32_t USART_Control (      uint32_t          control,
                                            uint32_t          arg,
                                      const USART_RESOURCES  *usart)
  \brief       Control USART Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \param[in]   usart    Pointer to USART resources
  \return      common \ref execution_status and driver specific \ref usart_execution_status
*/
static int32_t USART_Control (      uint32_t          control,
                                    uint32_t          arg,
                              const USART_RESOURCES  *usart) {

  if ((usart->info->flags & USART_FLAG_POWERED) == 0U) {
    // USART not powered
    return ARM_DRIVER_ERROR;
  }

  switch (control & ARM_USART_CONTROL_Msk) {
    // Control break
    case ARM_USART_CONTROL_BREAK:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    // Abort Send
    case ARM_USART_ABORT_SEND:
      // TX interrupt disable
      usart->reg->CTRL &= ~CMSDK_UART_CTRL_TXIRQEN_Msk;
      // Clear tx_busy status
      usart->info->status.tx_busy = 0U;
      return ARM_DRIVER_OK;

    // Abort receive
    case ARM_USART_ABORT_RECEIVE:
      // RX interrupt disable
      usart->reg->CTRL &= ~CMSDK_UART_CTRL_RXIRQEN_Msk;
      // Clear rx_busy status
      usart->info->status.rx_busy = 0U;
      return ARM_DRIVER_OK;

    // Abort transfer
    case ARM_USART_ABORT_TRANSFER:
      // Synchronous mode not supported
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    // Control TX
    case ARM_USART_CONTROL_TX:
      usart->info->flags |= USART_FLAG_TX_ENABLED;
      // Transmitter enable
      usart->reg->CTRL |= ((arg & 1U) << CMSDK_UART_CTRL_TXEN_Pos);
      return ARM_DRIVER_OK;

    // Control RX
    case ARM_USART_CONTROL_RX:
      usart->info->flags |= USART_FLAG_RX_ENABLED;
      // Receiver enable
      usart->reg->CTRL |= ((arg & 1U) << CMSDK_UART_CTRL_RXEN_Pos);
      return ARM_DRIVER_OK;
    default: break;
  }

  // Check if busy
  if ((usart->info->status.tx_busy != 0U) ||
      (usart->info->status.rx_busy != 0U) ||
     ((usart->reg->STATE & (CMSDK_UART_STATE_TXBF_Msk | CMSDK_UART_STATE_RXBF_Msk)) != 0U)) {
    return ARM_DRIVER_ERROR_BUSY;
  }

  switch (control & ARM_USART_CONTROL_Msk) {
    case ARM_USART_MODE_ASYNCHRONOUS: break;
    // Unsupported command
    default: return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  // USART Data bits
  switch (control & ARM_USART_DATA_BITS_Msk) {
    case ARM_USART_DATA_BITS_8: break;
    default: return ARM_USART_ERROR_DATA_BITS;
  }

  // USART Parity
  switch (control & ARM_USART_PARITY_Msk) {
    case ARM_USART_PARITY_NONE: break;
    default: return ARM_USART_ERROR_PARITY;
  }

  // USART Stop bits
  switch (control & ARM_USART_STOP_BITS_Msk) {
    case ARM_USART_STOP_BITS_1: break;
    default: return ARM_USART_ERROR_STOP_BITS;
  }

  // USART Flow control
  switch (control & ARM_USART_FLOW_CONTROL_Msk) {
    case ARM_USART_FLOW_CONTROL_NONE: break;
    default: return ARM_USART_ERROR_FLOW_CONTROL;
  }

  // USART Baudrate
  if ((SystemCoreClock/arg) < 16U)
    return ARM_USART_ERROR_BAUDRATE;
  else
    usart->reg->BAUDDIV = SystemCoreClock/arg;

  // Set configured flag
  usart->info->flags |= USART_FLAG_CONFIGURED;

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_USART_STATUS USART_GetStatus (const USART_RESOURCES *usart)
  \brief       Get USART status.
  \param[in]   usart     Pointer to USART resources
  \return      USART status \ref ARM_USART_STATUS
*/
static ARM_USART_STATUS USART_GetStatus (const USART_RESOURCES *usart) {
  ARM_USART_STATUS status;

  status.tx_busy          = usart->info->status.tx_busy;
  status.rx_busy          = usart->info->status.rx_busy;
  status.rx_overflow      = usart->info->status.rx_overflow;

  return status;
}

/**
  \fn          int32_t USART_SetModemControl (      ARM_USART_MODEM_CONTROL  control,
                                              const USART_RESOURCES         *usart)
  \brief       Set USART Modem Control line state.
  \param[in]   control   \ref ARM_USART_MODEM_CONTROL
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_SetModemControl (      ARM_USART_MODEM_CONTROL  control,
                                      const USART_RESOURCES         *usart) {
  (void)control;
  (void)usart;

  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn          ARM_USART_MODEM_STATUS USART_GetModemStatus (const USART_RESOURCES *usart)
  \brief       Get USART Modem Status lines state.
  \param[in]   usart     Pointer to USART resources
  \return      modem status \ref ARM_USART_MODEM_STATUS
*/
static ARM_USART_MODEM_STATUS USART_GetModemStatus (const USART_RESOURCES *usart) {
  ARM_USART_MODEM_STATUS modem_status = { 0U, 0U, 0U, 0U, 0U};

  (void)usart;

  return modem_status;
}

/**
  \fn          void USARTRX_IRQHandler (UART_RESOURCES *usart)
  \brief       USART RX Interrupt handler.
  \param[in]   usart     Pointer to USART resources
*/
static void USARTRX_IRQHandler (USART_RESOURCES *usart) {
  uint32_t status;

  status = usart->reg->INTSTATUS;
  if (usart->xfer->rx_buf != NULL)
    usart->xfer->rx_buf[usart->xfer->rx_cnt++] = (uint8_t)usart->reg->DATA;

  if (usart->xfer->rx_cnt >= usart->xfer->rx_num) {
    // RX is completed
    usart->xfer->rx_num = 0U;
    usart->info->status.rx_busy = 0U;

    // RX interrupt disable
    usart->reg->CTRL &= ~CMSDK_UART_CTRL_RXIRQEN_Msk;

    // Send event
    if (usart->info->cb_event!=NULL)
      usart->info->cb_event(ARM_USART_EVENT_RECEIVE_COMPLETE);
  }

  // clear interrupt
  usart->reg->INTCLEAR = status;
}

/**
  \fn          void USARTTX_IRQHandler (UART_RESOURCES *usart)
  \brief       USART TX Interrupt handler.
  \param[in]   usart     Pointer to USART resources
*/
static void USARTTX_IRQHandler (USART_RESOURCES *usart) {
  uint32_t status;

  // read interrupt status
  status = usart->reg->INTSTATUS;

  // clear interrupt
  usart->reg->INTCLEAR = status;

  if (usart->xfer->tx_num > 0) {

    usart->xfer->tx_cnt++;

    if (usart->xfer->tx_cnt < usart->xfer->tx_num) {
      // Send next byte
      usart->reg->DATA = usart->xfer->tx_buf[usart->xfer->tx_cnt];
    } else {
      // TX is completed
      usart->xfer->tx_num = 0U;
      usart->info->status.tx_busy = 0U;

      // TX interrupt disable
      usart->reg->CTRL &= ~CMSDK_UART_CTRL_TXIRQEN_Msk;

      // Send event
      if (usart->info->cb_event!=NULL)
        usart->info->cb_event(ARM_USART_EVENT_SEND_COMPLETE);
    }
  }

}

// USART0 Driver Wrapper functions
static ARM_USART_CAPABILITIES  USART0_GetCapabilities (void)                                                { return USART_GetCapabilities (&USART0_Resources); }
static int32_t                 USART0_Initialize      (ARM_USART_SignalEvent_t cb_event)                    { return USART_Initialize (cb_event, &USART0_Resources); }
static int32_t                 USART0_Uninitialize    (void)                                                { return USART_Uninitialize (&USART0_Resources); }
static int32_t                 USART0_PowerControl    (ARM_POWER_STATE state)                               { return USART_PowerControl (state, &USART0_Resources); }
static int32_t                 USART0_Send            (const void *data, uint32_t num)                      { return USART_Send (data, num, &USART0_Resources); }
static int32_t                 USART0_Receive         (void *data, uint32_t num)                            { return USART_Receive (data, num, &USART0_Resources); }
static int32_t                 USART0_Transfer        (const void *data_out, void *data_in, uint32_t num)   { return USART_Transfer (data_out, data_in, num, &USART0_Resources); }
static uint32_t                USART0_GetTxCount      (void)                                                { return USART_GetTxCount (&USART0_Resources); }
static uint32_t                USART0_GetRxCount      (void)                                                { return USART_GetRxCount (&USART0_Resources); }
static int32_t                 USART0_Control         (uint32_t control, uint32_t arg)                      { return USART_Control (control, arg, &USART0_Resources); }
static ARM_USART_STATUS        USART0_GetStatus       (void)                                                { return USART_GetStatus (&USART0_Resources); }
static int32_t                 USART0_SetModemControl (ARM_USART_MODEM_CONTROL control)                     { return USART_SetModemControl (control, &USART0_Resources); }
static ARM_USART_MODEM_STATUS  USART0_GetModemStatus  (void)                                                { return USART_GetModemStatus (&USART0_Resources); }
       void                    UART0RX_Handler        (void)                                                {        USARTRX_IRQHandler (&USART0_Resources); }
       void                    UART0TX_Handler        (void)                                                {        USARTTX_IRQHandler (&USART0_Resources); }

// USART0 Driver Control Block
extern ARM_DRIVER_USART Driver_USART0;
       ARM_DRIVER_USART Driver_USART0 = {
    USARTx_GetVersion,
    USART0_GetCapabilities,
    USART0_Initialize,
    USART0_Uninitialize,
    USART0_PowerControl,
    USART0_Send,
    USART0_Receive,
    USART0_Transfer,
    USART0_GetTxCount,
    USART0_GetRxCount,
    USART0_Control,
    USART0_GetStatus,
    USART0_SetModemControl,
    USART0_GetModemStatus
};
