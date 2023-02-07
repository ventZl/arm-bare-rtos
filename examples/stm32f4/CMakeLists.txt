cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE toolchain-arm-gcc.cmake)

# Find, where pico-sdk is installed. Is the path provided on commandline?
if (NOT STM_CUBE_PATH)
    # Do we have pico-sdk checked out locally?
    if (IS_DIRECTORY ${CMAKE_SOURCE_DIR}/STMCube32F4)
        set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/STMCube32F4)
    else()
        message(FATAL_ERROR "Path to STM32CubeF4 not defined! Please, define use -DSTM_CUBE_PATH=<path_to_stm_cube> to define where Raspberry Pi Pico SDK has been downloaded.\n\nAlternatively run git clone https://github.com/STMicroelectronics/STM32CubeF4 in source directory of this example.")
    endif()
endif()

# FindCMSIS will generate some CMSIS-related headers into build directory
include_directories(${CMAKE_BINARY_DIR})

# To get access to FindCMSIS
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../cmake")

# Path to kernel sources
set(OS_SRCS_ROOT ${CMAKE_SOURCE_DIR}/../../src)

# To be able to include kernel headers
include_directories(${OS_SRCS_ROOT})


set(CMSIS_ROOT ${STM_CUBE_PATH}/Drivers/CMSIS/)
set(DEVICE stm32f410cx)
set(CMSIS_STARTUP_FILTER gcc)
set(SYSTEM_SOURCE ${STM_CUBE_PATH}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c)
set(CMSIS_LINKER_FILE ${STM_CUBE_PATH}/Projects/STM32F410xx-Nucleo/Templates/SW4STM32/STM32F410Cx_Nucleo/STM32F410CBUx_FLASH.ld)

include(FindCMSIS)

project(bare-rtos-example C CXX ASM)

add_definitions(-Wall -Wextra)
add_definitions(-DSTM32F410Cx)
add_definitions(-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fomit-frame-pointer)
add_link_options(-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-T${CMSIS_LINKER_FILE})
add_executable(bare-rtos ${OS_SRCS_ROOT}/kernel/kernel.c main.c)
target_link_libraries(bare-rtos cmsis_interface cmsis_startup)
#pico_add_extra_outputs(bare-rtos)

#add_executable(bare-rtos-cpp ${OS_SRCS_ROOT}/kernel/kernel.c main.c)
#target_link_libraries(bare-rtos-cpp pico_stdlib cmsis_interface cmsis_core)
#pico_add_extra_outputs(bare-rtos-cpp)
