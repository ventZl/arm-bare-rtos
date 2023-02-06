message(STATUS "CMSIS root is: ${CMSIS_ROOT}")
message(STATUS "DEVICE is: ${DEVICE}")
message(STATUS "LIST dir is: ${CMAKE_CURRENT_SOURCE_DIR}")

# Find CMSIS components one by one
# These files shall exist in any CMSIS-compatible SDK, find them
file(GLOB_RECURSE DEVICE_INCLUDE ${CMSIS_ROOT}/*/${DEVICE}.h)
file(GLOB_RECURSE SYSTEM_INCLUDE ${CMSIS_ROOT}/*/system_${DEVICE}.h)
file(GLOB_RECURSE CORES_INCLUDE ${CMSIS_ROOT}/*/core_cm*.h)
file(GLOB_RECURSE SYSTEM_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMSIS_ROOT}/*/system_${DEVICE}.c)
file(GLOB_RECURSE STARTUP_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMSIS_ROOT}/*/startup_${DEVICE}.c)

message(STATUS "Device include: ${DEVICE_INCLUDE}")
message(STATUS "System include: ${SYSTEM_INCLUDE}")
message(STATUS "Cortex-M includes: ${CORES_INCLUDE}")
message(STATUS "System source: ${SYSTEM_SOURCE}")
message(STATUS "Startup source: ${STARTUP_SOURCE}")

# Extract paths to each of headers to compose CMSIS include directories 
foreach(FILE IN LISTS DEVICE_INCLUDE SYSTEM_INCLUDE CORES_INCLUDE)
    get_filename_component(INC_DIR "${FILE}" DIRECTORY)
    if (NOT "${INC_DIR}" IN_LIST INCLUDE_DIRS)
        list(APPEND INCLUDE_DIRS "${INC_DIR}")
    endif()
endforeach()

# Check that linker file has been specified
if (NOT CMSIS_LINKER_FILE)
    message(FATAL_ERROR "No linker script file provided in CMSIS_LINKER_FILE variable!")
endif()

# Check that specified linker file actually exists
if (NOT EXISTS ${CMSIS_LINKER_FILE})
    message(FATAL_ERROR "Linker script ${CMSIS_LINKER_FILE} not found!")
endif()

get_filename_component(CMSIS_LINKER_FILENAME "${CMSIS_LINKER_FILE}" NAME)
# Copy linker file into binary directory, so we can touch it
file(COPY_FILE ${CMSIS_LINKER_FILE} ${CMAKE_BINARY_DIR}/gen.${DEVICE}.ld)

# Generate generic include file stub
# This shall normally be provided by the SDK, but not all of them actually
# do this. This way, we can generate RTE_Components.h into build directory.
file(WRITE ${CMAKE_BINARY_DIR}/cmsis_conf.h 
    "#pragma once
"
    "#define CMSIS_device_header \"${DEVICE}.h\"
"
    )

# Copy file to well known name
file(COPY_FILE ${CMAKE_BINARY_DIR}/cmsis_conf.h ${CMAKE_BINARY_DIR}/RTE_Components.h)

# Create target, which if linked against, will provide CMSIS include directories
set(CMSIS_SRCS ${SYSTEM_SOURCE} ${STARTUP_SOURCE})
add_library(cmsis_interface INTERFACE)
set_property(TARGET cmsis_interface
    PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${INCLUDE_DIRS})


