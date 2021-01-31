# Low level PSX

Ps1 programming without SDK libraries.

Much inspired and taken from:

    https://github.com/PeterLemon/PSX
    https://github.com/iamgreaser/psx-chain
    https://github.com/ARM9/psxdev
    https://problemkaputt.de/psx-spx.htm

makefile assumes you have Lameguy 64's PSN00bSDK
(for the linker script and libc)

    https://github.com/Lameguy64/PSn00bSDK

The `raw_asm` directory contains a program to fill the screen white in main.asm
The `c_test` directory draws hello world with a textured font based on Peter Lemon's
hello world assembly example

