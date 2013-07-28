karma
=====

Dumb C++11 ARM kernel


This is a personal project to stay in touch with kernel programming. I decided
to do the kernel in C++11 and on ARM board because I saw nothing like this
existing (a lot of kernel exist, even in C++ but not that much for an ARM
board).

To compile the project you will need a cross C++11 compiler. I use GCC 4.8
with libgcc.

I don't have that much time to continue to work on this kernel at the moment.

My next steps will be to write a memory allocator, and an interrupt handler.

The code is designed to be run on PandaBoard ES (OMAP4460).
