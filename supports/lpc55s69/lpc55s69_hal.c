#include "fsl_device_registers.h"
// #include "fsl_debug_console.h"
#include "fsl_rng.h"
#include "arm_cmse.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "tzm_config.h"

#include "fsl_power.h"

void SystemInitHook(void)
{
    /* The TrustZone should be configured as early as possible after RESET.
     * Therefore it is called from SystemInit() during startup. The SystemInitHook() weak function
     * overloading is used for this purpose.
     */
    BOARD_InitTrustZone();
}

void HARM_HAL_Device_Init(void)
{
    /* Init board hardware. */
    /* set BOD VBAT level to 1.65V */
    POWER_SetBodVbatLevel(kPOWER_BodVbatLevel1650mv, kPOWER_BodHystLevel50mv, false);
    /* attach main clock divide to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitPins();
    BOARD_BootClockPLL150M();
    BOARD_InitDebugConsole();
}

void HARM_HAL_SecureRNG_Init(void)
{
    RNG_Init(RNG);
}

int HARM_HAL_SecureRNG_GetNext(void)
{
    uint32_t data;
	status_t status = RNG_GetRandomData(RNG, &data, 4);
    return status == kStatus_Success ? data : -1;
}