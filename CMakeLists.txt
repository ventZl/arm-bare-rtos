cmake_minimum_required(VERSION 3.12)

set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/../pico-sdk)

include_directories(${PICO_SDK_PATH})
include_directories(${CMAKE_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH "${PICO_SDK_PATH}/external")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(pico_sdk_import)

set(CMSIS_ROOT ${PICO_SDK_PATH}/src/rp2_common/cmsis/stub/CMSIS)
set(DEVICE RP2040)
set(CMSIS_LINKER_FILE ${PICO_SDK_PATH}/src/rp2_common/pico_standard_link/memmap_default.ld)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb3")

include(FindCMSIS)

project(nano-rtos-example C CXX ASM)

pico_sdk_init()


add_executable(nano-rtos src/kernel/kernel.c main.c)
target_link_libraries(nano-rtos pico_stdlib cmsis_interface cmsis_core)
