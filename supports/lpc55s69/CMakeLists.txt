aux_source_directory(board LIB_SRCS)
aux_source_directory(component LIB_SRCS)
aux_source_directory(component/lists LIB_SRCS)
aux_source_directory(component/uart LIB_SRCS)
aux_source_directory(device LIB_SRCS)
aux_source_directory(drivers LIB_SRCS)
aux_source_directory(startup LIB_SRCS)
aux_source_directory(trustzone LIB_SRCS)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/board"
    "${CMAKE_CURRENT_SOURCE_DIR}/device"
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers"
    "${CMAKE_CURRENT_SOURCE_DIR}/component/lists"
    "${CMAKE_CURRENT_SOURCE_DIR}/component/uart"
    "${CMAKE_CURRENT_SOURCE_DIR}/trustzone"
    "${CMAKE_CURRENT_SOURCE_DIR}/startup"
)

add_definitions(-DCPU_LPC55S69JBD100_cm33_core0)
add_definitions(-DMCUXPRESSO_SDK)
add_definitions(-DCPU_LPC55S69JBD100)
add_definitions(-DCPU_LPC55S69JBD100_cm33)
add_definitions(-DSDK_DEBUGCONSOLE=1)
add_definitions(-DCR_INTEGER_PRINTF)
add_definitions(-D__MCUXPRESSO)
add_definitions(-D__USE_CMSIS)
add_definitions(-DDEBUG)
add_definitions(-D__NEWLIB__)

add_library(lpc55s69 ${LIB_DIRS})