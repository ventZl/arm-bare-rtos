Helper CMake module to find CMSIS in your Cortex-M SDK
======================================================

This script will try to find all necessary components of CMSIS inside
your SDK distribution. There are couple components we need:
 * device header file, usually named <device>.h
 * system header file, usually named system_<device>.h
 * system source file, usually named system_<device>.c 
 * startup source file, usually named startup_<device>.c / S / s
 * Cortex-M header files, usually named core_cm<number>.h

Then we need to know which linker file is going to be used. This has to 
be determined by the user.

This helper script will provide two libraries:

cmsis_interface
---------------
This one provides include paths to all detected header files. Simply link 
it to your binary and it will have all the include paths to CMSIS headers 
available. You can include all files without actually bothering to locate
them.

cmsis_startup
-------------
This one provides actual startup and system code. If you link it to your
binary, startup and system code will be linked. The reason, why this is 
separated from interface library is, that a) some SDKs, such as pico-sdk
have some stuff in system library dependant on HAL headers. So you have
to link the whole sdk library. b) you may want to use your own startup
code.

Aside from two targets, this code also creates two files. CMSIS standard
requires a file named RTE_Components.h be created somewhere in the include
path. This file defines CMSIS_device_header, which points to actual location
of CMSIS device header (the one <device>.h). You can simply include 
RTE_Components.h and then have line

#include CMSIS_device_header

And device header for the used device will automatically be linked. this
makes the code a bit more portable. This file is created in the root of
build directory, so you can (shall) add include_directories(${CMAKE_BINARY_DIR})
to your top-level CMakeLists.txt to be able to use it.

Usage
=====

To use this module, first define a few variables:

CMSIS_ROOT
----------
The root directory of your SDK, if too large, some of subdirectories which contains
all the CMSIS components.

DEVICE
------
The name of your MCU. It is case sensitive.

CMSIS_LINKER_FILE
-----------------
Path to the linker file.

Then simply include FindCMSIS.cmake. It will try to find all the components needed
and creates two targets for further use.

Advanced use
============

Many of SDKs have some deviations and not all features can be detected automatically.
For such cases, there are advanced options. You can define any of the following variables
to leverage these options:

STARTUP_SOURCE
--------------
Define this variable in order to hardcode filename of startup source for given MCU.
This is the file usually named startup_<device>.c, .S or .s. In rare cases, the file 
does not follow this naming pattern and cannot be found automatically. Automatic detection
will be skipped for this file, if variable is set prior including FindCMSIS.

SYSTEM_SOURCE
-------------
Define this variable in order to hardcode filename of system source for given MCU.
This is the file usually named startup_<device>.c but sometimes this file is shared between
multiple MCUs and the device part does not match device name exactly, or entirely. In such
cases you can enforce the file name here. Automatic detection will be skipped for this
file, if variable is set before including FindCMSIS.

CMSIS_SYSTEM_FILTER
-------------------
Sometimes it happens that SDK supports multiple compilers and there are multiple copies
of certain files. You only want to include some of them. You can set this variable to 
contain regex, which will be used to filter system source files to only contain those
relevant for your toolchain.

CMSIS_STARTUP_FILTER
--------------------
Quite often, startup files are duplicated and there are multiple copies, one for each 
toolchain, which is supported. You can use this variable to filter out only files 
relevant for your toolchain. Set it to any non-empty regex to filter out only files
relevant to you.

