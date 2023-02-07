Nano RTOS examples
==================

This directory contains small collection of various examples. They tend to be small
pieces of code which show how to integrate RTOS code with extremely minimalistic 
subset of various vendor SDKs.

As of now, there are examples of how to build nano RTOS on top of following SDKs:

 * STM32Cube - CubeF4 was chosen purely randomly, but similar approach shall be usable
   with all Cube versions. Just use Cube version for your MCU family and update device
   ID. You'll have to be creative with the linker script as Cube does not contains
   templates for linker scripts in all cases.
 * nRF5 SDK - Here, 52832 was chosen as it is present in Pine64 Pinetime, so you can
   actually see what this code does. Basically any device supported by this SDK shall
   work out of box.
 * Raspberry Pi pico-sdk which supports RP2040 MCU. Here you have to link the whole
   SDK. You cannot link just cmsis_startup library due to the way how external FLASH
   is supported. Also, as this SDK is built on top of CMake, you basically have to 
   use CMake to build your project.

All examples are accompanied by CMakeLists.txt as of now. These files have toolchain
files bundled to add support for arm-gcc cross compiler. These files assume that
you have arm-none-eabi-gcc in your PATH environment variable (at least during the first
run of CMake).

These examples rely on presence of external components, such as STM32Cube, nRF5 SDK 
or pico-sdk. We don't submodule these, as this repo is lean enough to be directly
submodules by its users. Having submodules, it would create a risk of dragging tons
of dependencies in case someone used `git submodule update --init --recursive`.

You are free to ignore CMake presence and integrate RTOS in any other way which suits you.
