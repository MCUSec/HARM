# Add MCU Supports

To add a support for another MCU, please do as the following steps.

1. Create a new directory under `supports` directory with the name of your target board

    ```bash
    mkdir target # please change target as the name of your board
    ```

2.  In the direcotry of your target MCU, the following three files must be included:

    | file name | description |
    | ----- | ---- |
    | `target_hal.c` | Initialization code of the target board |
    | `target.ld` | Linker script |
    | `CMakeLists.txt` | Bulid script for `CMAKE` |

3. Implement the following functions `HARM_HAL_Device_Init`, `HARM_HAL_SecureRNG_Init`, `HARM_HAL_SecureRNG_GetNext` in `target_hal.c`.

    - `void HARM_HAL_Device_Init(void)`: Initialize the target board, including memory partitioning, access permission enforcement, clock configuration and so on.

    - `void HARM_HAL_SecureRNG_Init(void)`: Initialize the random number generator (RNG).

    - `int HARM_HAL_SecureRNG_GetNext(void)`: Get a random number from the RNG.

    Template of `target_hal.c`:

    ```C
    /* Include your header files */
    ...
    void HARM_HAL_Device_Init(void)
    {
        /* Code for memory partitioning, access permission enforcement*/
        ...
        /* Code for clock configuration */
        ...
        /* Code for debug console initialization */
        ...
    }

    void HARM_HAL_SecureRNG_Init(void)
    {
        /* Code for RNG initializaion*/
    }

    int HARM_HAL_SecureRNG_GetNext(void)
    {
        /* Code for getting a random number from RNG pool */
    }
    ``` 

4. Describe the memory layout in the linker script file `target.ld`. **NOTE**: `.gnu.sgstubs` section must be contained to place the non-secure callable (NSC) veneer table.

    ```C
    SECTIONS
    {
        ...  /* other sections */
        .gnu.sgstubs:
        {
            __start_sg__ = .
            *(.gnu.sgstubs*)
            __end_sg__ .
        } > FLASH
        ... /* other sections */
    }
    ```

5. Edit `CMakeLists.txt`:

    ```CMAKE
    project(<target_name>)
    ...
    include_directories(...)
    add_definitions(-DFOO -DBAR ...)
    aux_source_directory(... LIB_SRCS)
    ...
    add_library(${PROJECT_NAME} ${LIB_SRCS})

    ```

6. Edit `CMakeLists.txt` in the root directory:
    ```CMAKE
    ...
    option(TARGET_<YOUR_BOARD_NAME> "Target your board" Off)

    if (TARGET_YOUR_BOARD_NAME)
        add_subdirectory(supports/<target_name>)
        set(EXTRA_LIBS ${EXTRA_LIBS} <target_name>)
    endif (TARGET_<YOUR_BOARD_NAME>)
    ...
    ```