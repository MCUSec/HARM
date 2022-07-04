/*
 * Copyright 2017-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "board.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    RESET_ClearPeripheralReset(BOARD_DEBUG_UART_RST);

    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;

    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
}

void BOARD_InitDebugConsole_Core1(void)
{
    /* attach 12 MHz clock to FLEXCOMM1 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH_CORE1);

    RESET_ClearPeripheralReset(BOARD_DEBUG_UART_RST_CORE1);

    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ_CORE1;

    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE_CORE1, BOARD_DEBUG_UART_BAUDRATE_CORE1, BOARD_DEBUG_UART_TYPE_CORE1,
                    uartClkSrcFreq);
}
