/*
 * Trace Recorder Stream Port Configuration for STM32 HAL UART
 *
 * This stream port uses an STM32 HAL UART peripheral for bidirectional
 * communication with Tracealyzer. The user must define TRACE_UART to
 * the desired UART_HandleTypeDef instance (e.g. huart1) before including
 * trcRecorder.h, or in trcConfig.h.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TRC_STREAM_PORT_CONFIG_H
#define TRC_STREAM_PORT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * TRACE_UART
 *
 * Specify which UART handle to use, e.g.:
 *   #define TRACE_UART huart1
 *
 * This must be a UART_HandleTypeDef that has been initialized (MX_USARTx_UART_Init)
 * before xTraceEnable() is called.
 ******************************************************************************/
#ifndef TRACE_UART
#error "TRACE_UART is not defined. Please define it to your UART handle, e.g. #define TRACE_UART huart1"
#endif

/**
 * @def TRC_CFG_STREAM_PORT_DELAY_ON_BUSY
 *
 * @brief The time to wait if the USB interface is busy.
 */
#define TRC_CFG_STREAM_PORT_DELAY_ON_BUSY 4

/*******************************************************************************
 * TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
 *
 * When set to 1, an internal buffer is used by the recorder to store trace
 * data before it is flushed to the UART in the TzCtrl task. This is
 * recommended for UART streaming to smooth out bursts.
 *
 * Default: 1
 ******************************************************************************/
#ifndef TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
#define TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER 1
#endif

/*******************************************************************************
 * TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE
 *
 * Size in bytes of the internal event buffer used when
 * TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER is 1.
 *
 * Default: 5120
 ******************************************************************************/
#ifndef TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE 5120
#endif

/*******************************************************************************
 * TRC_CFG_STREAM_PORT_UART_TRANSMIT_TIMEOUT_MS
 *
 * HAL_UART_Transmit timeout in milliseconds per call.
 * Keep this short to avoid blocking the TzCtrl task too long.
 *
 * Default: 5
 ******************************************************************************/
#ifndef TRC_CFG_STREAM_PORT_UART_TRANSMIT_TIMEOUT_MS
#define TRC_CFG_STREAM_PORT_UART_TRANSMIT_TIMEOUT_MS 20
#endif

/*******************************************************************************
 * TRC_CFG_STREAM_PORT_UART_RECEIVE_TIMEOUT_MS
 *
 * HAL_UART_Receive timeout in milliseconds per call.
 * This is called periodically by TzCtrl to check for host commands.
 * Keep it very short (non-blocking or near-zero) so it doesn't stall.
 *
 * Default: 1
 ******************************************************************************/
#ifndef TRC_CFG_STREAM_PORT_UART_RECEIVE_TIMEOUT_MS
#define TRC_CFG_STREAM_PORT_UART_RECEIVE_TIMEOUT_MS 1
#endif

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAM_PORT_CONFIG_H */
