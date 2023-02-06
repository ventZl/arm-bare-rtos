Raspberry Pi Pico example
=========================

This is an example of bare RTOS use on RP2040 using the pico-sdk. This example
defines two threads, one permanently activates the built-in LED, another
disables it, creating very fancy way of doing LED blinks. Other than this, the
example shows user code that hooks SysTick as the clock provider for preemptive
multi-threading.

Building this example
---------------------

To build this example, you need to have Raspberry Pi Pico SDK in recent-enough
version deployed on your machine. There are two possible options.

It is possible, that you already have pico-sdk deployed somewhere on your drive.
In such case, it is possible to use this copy of pico-sdk. To do so, pass 

~~~~~~
-DPICO_SDK_PATH=<path-to-pico-sdk>
~~~~~~

to CMake, when configuring this project. Make sure that pico-sdk is able to
find your ARM compiler toolchain.

Alternatively, you can clone pico-sdk directly into this directory using:

~~~~~
git clone https://github.com/raspberrypi/pico-sdk
~~~~~

And it will be automatically used.

Testing
-------

Simplest way of testing this is to upload the bare-rtos.uf2 file onto Pico.
Unplug USB connector from pico, push the BOOTSEL button and reconnect USB back.
This will create new USB storage-like device where you can upload the UF2
binary. After you do so, Raspberry Pi Pico will reboot and should start blinking
at rather rapid pace. This means that the example works as expected.

