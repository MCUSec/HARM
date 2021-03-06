cmake_minimum_required (VERSION 2.6)
project (harm_secure_rt)
set(CMAKE_SYSTEM_NAME "Generic")

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_LINKER "arm-none-eabi-ld" CACHE PATH "" FORCE)

set(CMAKE_C_FLAGS "-std=c99 -Wall -ffunction-sections -fdata-sections -fno-exceptions \
                    -ffreestanding -fno-builtin -mthumb -mcmse -fstack-usage")

set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -Wl,--cmse-implib -Wl,--out-implib=secure_rt_nsc.o \
                            --specs=nano.specs --specs=nosys.specs \
                            -Wl,-Map=${PROJECT_NAME}.map \
                            -Wl,--gc-sections -Wl,-print-memory-usage \
                            -Wl,--sort-section=alignment \
                            -Wl,--cref -mthumb")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/utils")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/CMSIS")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

aux_source_directory(. SRCS)
aux_source_directory(utils SRCS)

option(TARGET_LPC55S69 "Target NXP LPC55s69 Development Board" ON)

if (TARGET_LPC55S69)
    add_subdirectory(supports/lpc55s69)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-mfpu=fpv5-sp-d16 -mfloat-abi=hard")
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} 
            "-mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard"
            "-T ../supports/lpc55s69/lpc55s69.ld")
    set(EXTRA_LIBS ${EXTRA_LIBS} lpc55s69)
endif (TARGET_LPC55S69)

add_executable(${PROJECT_NAME}.elf ${SRCS})
target_link_libraries(${PROJECT_NAME}.elf ${EXTRA_LIBS})

