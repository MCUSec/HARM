cmake_minimum_required (VERSION 2.6)
project (harm_secure_rt)
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_FLAGS "-std=c99")

include_directories("${PROJECT_BINARY_DIR}/utils")
include_directories("${PROJECT_BINARY_DIR}/CMSIS")
include_directories("${PROJECT_BINARY_DIR}")

aux_source_directory(. SRCS)

option(TARGET_LPC55S69 "Target NXP LPC55s69 Development Board" ON)

if (TARGET_LPC55S69)
    add_subdirectory(supports/lpc55s69)
    set(EXTRA_LIBS ${EXTRA_LIBS} lpc55s69)
endif (TARGET_LPC55S69)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS})

