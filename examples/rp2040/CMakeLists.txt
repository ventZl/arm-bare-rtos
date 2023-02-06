cmake_minimum_required(VERSION 3.12)

# Find, where pico-sdk is installed. Is the path provided on commandline?
if (NOT PICO_SDK_PATH)
    # Do we have pico-sdk checked out locally?
    if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/pico-sdk)
        set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/pico-sdk)
    else()
        message(FATAL_ERROR "Path to pico-sdk not defined! Please, define use -DPICO_SDK_PATH=<path_to_pico_sdk> to define where Raspberry Pi Pico SDK has been downloaded.\n\nAlternatively run git clone https://github.com/raspberrypi/pico-sdk in source directory of this example.")
    endif()
endif()

include_directories(${PICO_SDK_PATH})

# FindCMSIS will generate some CMSIS-related headers into build directory
include_directories(${CMAKE_BINARY_DIR})

# To get access to pico-sdk stuff without actually copying it over
list(APPEND CMAKE_MODULE_PATH "${PICO_SDK_PATH}/external")

# To get access to FindCMSIS
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../cmake")

# Include pico-sdk
include(pico_sdk_import)

# Path to kernel sources
set(OS_SRCS_ROOT ${CMAKE_SOURCE_DIR}/../../src)

# To be able to include kernel headers
include_directories(${OS_SRCS_ROOT})


set(CMSIS_ROOT ${PICO_SDK_PATH}/src/rp2_common/cmsis/stub/CMSIS)
set(DEVICE RP2040)
set(CMSIS_LINKER_FILE ${PICO_SDK_PATH}/src/rp2_common/pico_standard_link/memmap_default.ld)

include(FindCMSIS)

project(bare-rtos-example C CXX ASM)

pico_sdk_init()

add_executable(bare-rtos ${OS_SRCS_ROOT}/kernel/kernel.c main.c)
target_link_libraries(bare-rtos pico_stdlib cmsis_interface cmsis_core)
pico_add_extra_outputs(bare-rtos)
