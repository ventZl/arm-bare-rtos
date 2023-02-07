cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE toolchain-arm-gcc.cmake)

# Find, where pico-sdk is installed. Is the path provided on commandline?
if (NOT NRF_SDK_PATH)
    # Do we have pico-sdk checked out locally?
    message(FATAL_ERROR "Path to nRF5 SDK not defined! Please, define use -DNRF_SDK_PATH=<path_to_stm_cube> to define where it is installed.")
endif()

# FindCMSIS will generate some CMSIS-related headers into build directory
include_directories(${CMAKE_BINARY_DIR})

# To get access to FindCMSIS
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../cmake")

# Path to kernel sources
set(OS_SRCS_ROOT ${CMAKE_SOURCE_DIR}/../../src)

# To be able to include kernel headers
include_directories(${OS_SRCS_ROOT})

# Vast majority of CMSIS lives under /modules/nrfx/mdk, but Cortex headers
# are elsewhere, search the whole nRF SDK here
set(CMSIS_ROOT ${NRF_SDK_PATH})

# nrf52832 is an alias to nrf52 
set(DEVICE nrf52)

# nRF SDK has startups for multiple toolchains, filter out for gcc
set(CMSIS_STARTUP_FILTER gcc)

# We will use provided linker script directly
set(CMSIS_LINKER_FILE ${NRF_SDK_PATH}/modules/nrfx/mdk/nrf52832_xxaa.ld)

# We have to specify these manually -fomit-frame-pointer is required for
# thread swticher to be able to touch r7
add_definitions(-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -fomit-frame-pointer)

# Link options have to match compile options for these, so the compiler
# can choose the correct multi-lib variant
add_link_options(-mcpu=cortex-m4 -mthumb -mfloat-abi=hard)

# Find CMSIS here
include(FindCMSIS)

# Define project, initialize the compiler
project(bare-rtos-example C CXX ASM)

# To bash all the warnings
add_definitions(-Wall -Wextra)

# NRF SDK wants this
add_definitions(-DNRF52)

# Define our firmware
add_executable(bare-rtos ${OS_SRCS_ROOT}/kernel/kernel.c main.c)

# Link it with interface library and with startup library
target_link_libraries(bare-rtos cmsis_interface cmsis_startup)

