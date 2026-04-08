/*
 * Trace Recorder Stream Port for STM32 HAL UART — Implementation
 *
 * Provides xTraceStreamPortInitialize, xTraceStreamPortWriteData and
 * xTraceStreamPortReadData backed by HAL_UART_Transmit / HAL_UART_Receive.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <trcRecorder.h>
#include "stm32u5xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"

#if (TRC_USE_TRACEALYZER_RECORDER == 1) && (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/*
 * TRACE_UART is expected to be the name of a UART_HandleTypeDef that the
 * application has already initialised (e.g. via MX_USART1_UART_Init()).
 * It is defined by the user in trcStreamPortConfig.h or trcConfig.h, e.g.:
 *
 *   #define TRACE_UART huart1
 *
 * We access it through an extern declaration so the linker resolves it
 * against the application's actual handle.
 */
extern UART_HandleTypeDef TRACE_UART;
static SemaphoreHandle_t traceSem;


void UART_Callback(UART_HandleTypeDef *huart);

void UART_Callback(UART_HandleTypeDef *huart)
{
//	xSemaphoreGive(traceSem);
}
/*******************************************************************************
 * xTraceStreamPortInitialize
 *
 * Called once by the recorder core during xTraceEnable().
 * The UART peripheral must already be configured and started by the
 * application at this point — we only store the buffer pointer for
 * potential future use and mark the component as initialised.
 ******************************************************************************/
traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
    /* Nothing UART-specific to do here — the peripheral is already running.
     * If we were using DMA we might set up the RX circular buffer here. */
    (void)pxBuffer;
//    traceSem = xSemaphoreCreateBinary();
//    if(traceSem == NULL)
//    	return TRC_FAIL;
//    xSemaphoreGive(traceSem);
//    HAL_UART_RegisterCallback(&TRACE_UART,HAL_UART_TX_COMPLETE_CB_ID,UART_Callback);
    return TRC_SUCCESS;
}


/*******************************************************************************
 * xTraceStreamPortWriteData
 *
 * Transmit trace data over the UART.
 *
 * This function is called from the TzCtrl task. It uses the blocking
 * HAL_UART_Transmit with a short timeout. On success, *piBytesWritten
 * is set to uiSize. On timeout or error it is set to 0 so the recorder
 * can retry on the next cycle.
 *****************************************************************************
 */

volatile uint32_t uartUsed = 0;
traceResult prvTraceUartWrite(void* pvData, uint32_t uiSize, uint32_t uiCoreId, int32_t* piBytesWritten)
{
	(void)uiCoreId;
    HAL_StatusTypeDef status;
    static int fail_counter = 0;

    if (pvData == (void*)0 || piBytesWritten == (void*)0)
    {
        return TRC_FAIL;
    }

    if (uiSize == 0u)
    {
        *piBytesWritten = 0;
        return TRC_SUCCESS;
    }

    //status = HAL_UART_Transmit_DMA(&TRACE_UART, (uint8_t*)pvData, (uint16_t)uiSize);

    status = HAL_UART_Transmit(
        &TRACE_UART,
        (uint8_t*)pvData,
        (uint16_t)uiSize,
        (uint32_t)TRC_CFG_STREAM_PORT_UART_TRANSMIT_TIMEOUT_MS
    );

    if (status == HAL_OK)
    {
        *piBytesWritten = (int32_t)uiSize;
        fail_counter = 0;
    }
    else
    {
    	fail_counter++;
        /* HAL_TIMEOUT or HAL_ERROR — report zero bytes so the recorder
         * keeps the data and retries next time around. */
        *piBytesWritten = 0;
        if (fail_counter >= 100)
        {
        	/* If many unsuccessful attempts in a row, something is very wrong. Returning -1 will stop the recorder. */
        	return TRC_FAIL;
        }
    }
    uartUsed = 0;
    return TRC_SUCCESS;
}

/*******************************************************************************
 * xTraceStreamPortReadData
 *
 * Receive host commands from the UART.
 *
 * Called periodically by the TzCtrl task. We attempt a short blocking
 * receive for one byte at a time (up to uiSize bytes) so we never
 * block longer than the configured timeout.
 *
 * A polling/non-blocking approach is used: we try to read one byte with
 * a very short timeout, and repeat until there is no more data or uiSize
 * has been satisfied. This avoids long blocking waits when the host is
 * silent.
 ******************************************************************************/
traceResult prvTraceUartRead(void* pvData, uint32_t uiSize, int32_t* piBytesRead)
{
    HAL_StatusTypeDef status;
    uint8_t* pucDst;
    uint32_t uiBytesReceived = 0u;

    if (pvData == (void*)0 || piBytesRead == (void*)0)
    {
        return TRC_FAIL;
    }

    pucDst = (uint8_t*)pvData;

    while (uiBytesReceived < uiSize)
    {
        status = HAL_UART_Receive(
            &TRACE_UART,
            &pucDst[uiBytesReceived],
            1u,
            (uint32_t)TRC_CFG_STREAM_PORT_UART_RECEIVE_TIMEOUT_MS
        );

        if (status == HAL_OK)
        {
            uiBytesReceived++;
        }
        else
        {
            /* HAL_TIMEOUT — no more data available right now. */
            break;
        }
    }

    *piBytesRead = (int32_t)uiBytesReceived;

    return TRC_SUCCESS;
}

#endif /* TRC_USE_TRACEALYZER_RECORDER == 1 && TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING */
