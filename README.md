Nano RTOS
=========

This is "bare" RTOS. Smallest-possible Cortex-M thread switcher you can write in 
C or C++. Originally the scheduler was mostly written in C with just a few helper
function written in assembly to perform some sensitive tasks which couldn't be
written in C. Later a C++20 module version was added.

It does only do the heavy lifting needed to perform thread switching and lets the
user to decide on everything else. It doesn't even implement any specific
scheduling model, so in fact you can extend it and have whatever scheduler
model, either preemptive, or cooperative; round robin or prioritized RT
scheduling.

Resulting in small memory and flash footprint allows you to have the luxury of
running threads even in more advanced flash bootloaders. It also has barely any
dependencies. All that this code needs is a CMSIS header to grab some structs
out of it. It should therefore be a quick drop-in addition to any project where
thread switching is needed.

Usage
-----

To actually use nano RTOS, all you have to do is to use the contents of
[src/kernel](src/kernel) subdirectory. There is one C, one C++ file and a bunch of 
headers. Cloning this repository as a submodule will totally work too. 
This repository does not have any submodules defined and contains only small "dead"
code in examples subdirectory . Everything else here is entirely optional.

There are two versions of the code provided:

 * kernel.c + api.h + arm_arch.h + kernel.h - purely C-based implementation
 * kernel.cpp + arm_arch.h - C++20 module-based implementation

You can use whichever you like, they are entirely independent from each other
(except of arm_arch.h, which contains shared intrinsics).

Aside from that, you'll have to provide a thread table variable and one
call-back method, which provides kernel with amount of available entries in the 
thread table.

Documentation
-------------

The API of both implementations is well documented by Doxygen comments inside the 
code. We have some additional documentation inside [doc](doc) subdirectory.

Example
-------

In [examples](examples) subdirectory, there is an example of how to use the kernel to
create an environment with preemptive multi-threading. More examples to come.
