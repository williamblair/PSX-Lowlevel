# Low level PSX

Ps1 programming without a SDK.

Much inspired and taken from:

    https://github.com/iamgreaser/psx-chain
    https://github.com/ARM9/psxdev
    https://problemkaputt.de/psx-spx.htm

At the moment the makefile assumes you have psxsdk installed
(for the linker script)

    http://unhaut.x10host.com/psxsdk/

Other than that, you need mipsel-none-elf or equivalent toolchain
installed plus newlib (I think)

I somewhat followed the instructions from here to install them:

    http://www.hs-augsburg.de/homes/beckmanf/dokuwiki/doku.php?id=mips_cross_compiler

But just installed Binutils, gcc, then newlib

