message(STATUS "CMSIS root is: ${CMSIS_ROOT}")
message(STATUS "DEVICE is: ${DEVICE}")

# Find CMSIS components one by one
# These files shall exist in any CMSIS-compatible SDK, find them
file(GLOB_RECURSE DEVICE_INCLUDE ${CMSIS_ROOT}/*/${DEVICE}.h)
file(GLOB_RECURSE SYSTEM_INCLUDE ${CMSIS_ROOT}/*/system_${DEVICE}.h)
file(GLOB_RECURSE CORES_INCLUDE ${CMSIS_ROOT}/*/core_cm*.h)

# Search for system source if it was not provided manually
if ("${SYSTEM_SOURCE}" STREQUAL "")
    file(GLOB_RECURSE SYSTEM_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
        ${CMSIS_ROOT}/*/system_${DEVICE}.c
        ${CMSIS_ROOT}/*/system_${DEVICE}.s
    )
else()
    message(STATUS "Skipping system source search(${SYSTEM_SOURCE})...")
endif()

# Search for startup source if it was not provided manually
if ("${STARTUP_SOURCE}" STREQUAL "")
    file(GLOB_RECURSE STARTUP_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
        ${CMSIS_ROOT}/*/*startup_${DEVICE}.s
        ${CMSIS_ROOT}/*/*startup_${DEVICE}.S
        ${CMSIS_ROOT}/*/*startup_${DEVICE}.c
    )
else()
    message(STATUS "Skipping startup source search (${STARTUP_SOURCE})...")
endif()

message(STATUS "Device include: ${DEVICE_INCLUDE}")
message(STATUS "System include: ${SYSTEM_INCLUDE}")
message(STATUS "Cortex-M includes: ${CORES_INCLUDE}")

# Some SDKs provide startup files for multiple toolchains, let them to be filtered
if (CMSIS_SYSTEM_FILTER)
    list(FILTER SYSTEM_SOURCE INCLUDE REGEX ${CMSIS_SYSTEM_FILTER})
endif()

if (CMSIS_STARTUP_FILTER)
    list(FILTER STARTUP_SOURCE INCLUDE REGEX ${CMSIS_STARTUP_FILTER})
endif()

message(STATUS "System source: ${SYSTEM_SOURCE}")

# Some SDKs provide startup files for multiple toolchains, let them to be filtered
if (CMSIS_STARTUP_FILTER)
        list(FILTER STARTUP_SOURCE INCLUDE REGEX ${CMSIS_STARTUP_FILTER})
endif()

message(STATUS "Startup source: ${STARTUP_SOURCE}")

# Extract paths to each of headers to compose CMSIS include directories 
foreach(FILE IN LISTS DEVICE_INCLUDE SYSTEM_INCLUDE CORES_INCLUDE)
    get_filename_component(INC_DIR "${FILE}" DIRECTORY)
    if (NOT "${INC_DIR}" IN_LIST INCLUDE_DIRS)
        list(APPEND INCLUDE_DIRS "${INC_DIR}")
    endif()
endforeach()

get_filename_component(LINK_DIR "${CMSIS_LINKER_FILE}" DIRECTORY)

# Check that linker file has been specified
if (NOT CMSIS_LINKER_FILE)
    message(FATAL_ERROR "No linker script file provided in CMSIS_LINKER_FILE variable!")
endif()

# Check that specified linker file actually exists
if (NOT EXISTS ${CMSIS_LINKER_FILE})
    message(FATAL_ERROR "Linker script ${CMSIS_LINKER_FILE} not found!")
endif()

get_filename_component(CMSIS_LINKER_FILENAME "${CMSIS_LINKER_FILE}" NAME)

# Generate generic include file stub
# This shall normally be provided by the SDK, but not all of them actually
# do this. This way, we can generate RTE_Components.h into build directory.
file(WRITE ${CMAKE_BINARY_DIR}/RTE_Components.h 
    "#pragma once
"
    "#define CMSIS_device_header \"${DEVICE}.h\"
"
    )

# Create target, which if linked against, will provide CMSIS include directories
set(CMSIS_SRCS ${SYSTEM_SOURCE} ${STARTUP_SOURCE})
add_library(cmsis_interface INTERFACE)
set_property(TARGET cmsis_interface
    PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIRS}")
set_property(TARGET cmsis_interface
    PROPERTY
    INTERFACE_COMPILE_OPTIONS)

# Add startup library, but only if there are any source for it
if ((NOT "${STARTUP_SOURCE}" STREQUAL "") OR (NOT "${SYSTEM_SOURCE}" STREQUAL ""))
    add_library(cmsis_startup STATIC EXCLUDE_FROM_ALL ${STARTUP_SOURCE} ${SYSTEM_SOURCE})
    set_property(TARGET cmsis_startup
        PROPERTY
        INTERFACE_LINK_OPTIONS "-T${CMSIS_LINKER_FILE}")
    target_link_directories(cmsis_startup INTERFACE ${LINK_DIR})
    target_link_libraries(cmsis_startup cmsis_interface)
endif()
