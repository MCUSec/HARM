#include "harm.h"

int main(void)
{
    HARM_HAL_Device_Init();
    HARM_HAL_SecureRNG_Init();
    HARM_Bootstrap();

    while (1);

    /* This point should never be reached */
    return 0;
}