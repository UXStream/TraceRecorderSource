/*
 * Trace Recorder Stream Port for STM32 HAL UART
 * Copyright 2025 — Custom stream port
 *
 * Provides bidirectional trace streaming over a user-specified STM32 HAL
 * UART peripheral. Compatible with Percepio TraceRecorder v4.8+.
 *
 * Usage:
 *   1. Define TRACE_UART to your UART handle before including trcRecorder.h:
 *        #define TRACE_UART huart1
 *   2. In trcConfig.h set:
 *        #define TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER 1
 *   3. Copy this streamport folder into your project and add:
 *        - trcStreamPort.c to your build
 *        - include/ and config/ to your include paths
 *   4. Make sure the UART is initialized before calling xTraceEnable().
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TRC_STREAM_PORT_H
#define TRC_STREAM_PORT_H

#include <trcTypes.h>
#include <trcStreamPortConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * NOTE: UART_HandleTypeDef must be visible at the point trcStreamPort.c is
 * compiled.  This is normally guaranteed because the user includes their
 * processor header (e.g. "stm32f4xx_hal.h") from trcConfig.h, which is
 * pulled in before trcStreamPort.h.  No additional #include is needed here.
 */

/*******************************************************************************
 * TraceStreamPortBuffer_t
 *
 * The recorder core embeds one instance of this type inside
 * TraceRecorderData_t (field xStreamPortBuffer). If the internal event
 * buffer is enabled, we use the framework's built-in buffer; otherwise
 * we need no extra storage and define an empty struct.
 ******************************************************************************/
#if (TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER == 1)

#include <trcInternalEventBuffer.h>

typedef struct TraceStreamPortBuffer
{
    uint8_t buffer[(TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE)];
} TraceStreamPortBuffer_t;

#else /* No internal buffer */

typedef struct TraceStreamPortBuffer
{
    uint8_t reserved; /* Empty structs are not valid C — keep one byte */
} TraceStreamPortBuffer_t;

#endif

/*******************************************************************************
 * Low-level read / write functions (implemented in trcStreamPort.c)
 ******************************************************************************/
traceResult prvTraceUartWrite(void* pvData, uint32_t uiSize, uint32_t uiCoreId, int32_t* piBytesWritten);
traceResult prvTraceUartRead(void* pvData, uint32_t uiSize, int32_t* piBytesRead);

/*******************************************************************************
 * xTraceStreamPortInitialize
 *
 * Called once during xTraceEnable(). The UART must already be initialised
 * by the application (CubeMX / HAL_UART_Init) before this point.
 ******************************************************************************/
traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer);

/*******************************************************************************
 * Stream port interface macros (active in streaming mode)
 *
 * These macros are the contract between the recorder core and the stream
 * port.  The v4.8+ recorder expects:
 *
 *   TRC_STREAM_PORT_WRITE_DATA(pvData, uiSize, piBytesWritten)
 *   TRC_STREAM_PORT_READ_DATA(pvData, uiSize, piBytesRead)
 *   TRC_STREAM_PORT_ON_TRACE_BEGIN()
 *   TRC_STREAM_PORT_ON_TRACE_END()
 ******************************************************************************/

/*
 * Write trace data out to the UART.
 * Called from the TzCtrl task context.
 * The recorder core calls this with 4 arguments:
 *   _pvData          - pointer to the data to transmit
 *   _uiSize          - number of bytes to transmit
 *   _uiCoreId        - core ID (unused on single-core targets, ignored here)
 *   _piBytesWritten  - [out] number of bytes actually written
 */

/*
 * Read incoming data (host commands) from the UART.
 * Called from the TzCtrl task context.
 */
#define xTraceStreamPortWriteData(_pvData, _uiSize, _uiCoreId, _piBytesWritten) prvTraceUartWrite(_pvData, _uiSize, _uiCoreId, _piBytesWritten)
#define xTraceStreamPortReadData(pvData, uiSize, piBytesRead) prvTraceUartRead(pvData, uiSize, piBytesRead)


#define xTraceStreamPortOnEnable(uiStartOption) (uiStartOption, TRC_SUCCESS)
#define xTraceStreamPortOnDisable() (TRC_SUCCESS)
#define xTraceStreamPortOnTraceBegin() (TRC_SUCCESS)
#define xTraceStreamPortOnTraceEnd() (TRC_SUCCESS)

/*
 * Called when tracing starts. Nothing special needed for UART since the
 * peripheral is already running.
 */
#define TRC_STREAM_PORT_ON_TRACE_BEGIN()

/*
 * Called when tracing stops. Nothing special needed.
 */
#define TRC_STREAM_PORT_ON_TRACE_END()

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAM_PORT_H */
