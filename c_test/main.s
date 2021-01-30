# PSX equs
.equ GP0, 0x1810 # GPU command ports 0 and 1
.equ GP1, 0x1814
.equ IO_BASE, 0x1F800000

# GP0 commands registers
.equ GP0_MODE,    0xE1     # draw mode setting
.equ GP0_TOPLEFT, 0xE3     # drawing top left X,Y
.equ GP0_B_RIGHT, 0xE4     # drawing bottom right X,Y
.equ GP0_OFFSET,  0xE5     # drawing area offset X,Y

# GP0 drawing commands
.equ GP0_FILLRECT, 0x02

# GP1 commands registers
.equ GP1_RESET,  0x00     # gpu display control 0x00 - reset GPU
.equ GP1_ENABLE, 0x03     # gpu display contol 0x03 - display enable
.equ GP1_HRANGE, 0x06     # gpu display control 0x06 - horizontal display range
.equ GP1_VRANGE, 0x07     # gpu display control 0x06 - vertical display range
.equ GP1_MODE,   0x08     # gpu display control 0x08 - display mode

# GP1_MODE parameters
.equ H_320, 0x01
.equ V_240, 0x00
.equ BPP_15,0x00
.equ NTSC,  0x00

# Write Register Word
.macro WriteRegWord regarg, data
    la $a0, IO_BASE
    li $t0, \data
    sw $t0, \regarg($a0)
.endm

# Write to GP0
.macro WriteGP0 command, param
    WriteRegWord GP0,((\command << 24)+(\param & 0xFFFFFF))
.endm

# Write to GP1
.macro WriteGP1 command, param
    WriteRegWord GP1,((\command << 24)+(\param & 0xFFFFFF))
.endm

    .section .mdebug.abi32
    .previous
    .nan    legacy
    .module softfloat
    .module nooddspreg
    .text
    .align  2
    .globl  main
	.set	nomips16
	.set	nomicromips
	.ent	main
	.type	main, @function
main:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp

    # BJ code starts here
	la $a0, IO_BASE

    # Init GPU and display mode
    WriteGP1 GP1_RESET,  0x00 # Reset GPU
    WriteGP1 GP1_ENABLE, 0x00 # Enable Display
    WriteGP1 GP1_MODE,   (H_320+V_240+BPP_15+NTSC) # set GPU mode 320x240, 15BPP, NTSC
    WriteGP1 GP1_HRANGE, 0xC60260 # horizontal display range 608..3168
    WriteGP1 GP1_VRANGE, 0x042018 # vertical display range 24..264

    # Set GPU drawing area
    WriteGP0 GP0_MODE,      0x000400 # Drawing to display area is allowed, bit 10
    WriteGP0 GP0_TOPLEFT,   0x000000 # Top left draw area X,Y = 0,0
    WriteGP0 GP0_B_RIGHT,   0x03BD3F # Bottom right draw area X,Y = 319,239
    WriteGP0 GP0_OFFSET,    0x000000 # Drawing offset X,Y = 0,0

FillRectVRAM:
    WriteGP0 GP0_FILLRECT, 0xFFFFFF # 0xF700 = color
    WriteRegWord GP0, (0x00 << 16)+(0x00 & 0xFFFF) # Y = 0, X = 0
    WriteRegWord GP0, (239 << 16) + (319 & 0xFFFF) # height = 239, width = 319

infLoop:
    b infLoop
    nop

    .end main
    
