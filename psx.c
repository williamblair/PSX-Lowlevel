/*
 * psx.c
 */

#include "psx.h"

void psx_wait_gpu(PSX *psx)
{
    uint8_t status;

    do {

        // get bit 28 from the status register
        status = ((*psx->gp1) >> 28) & 1;

    } while (!status); // bit 28 = 0 -> gpu is busy

    printf("Exiting waitgpu\n");
}

void psx_init(PSX *psx)
{
    psx->gp0 = (vu32*)GP0;
    psx->gp1 = (vu32*)GP1;
    
    // reset command 0x00
    *psx->gp1 = 0x00;
    psx_wait_gpu(psx);

    // Start of display area in VRAM
    // 0b00000101    0000    000000000   000000000
    //   command  | unused | Y (0-511) | X (0-1023)
    *psx->gp1 = 0x05000000;

    // Horizontal Display range (on screen)
    // 0b 00000110 000000000000 000000000000
    //    command |     X2     |     X1
    // the x2 and x1 are relative, but the formulas (I think) are:
    // X1:  always 0x260 (that's where most t.v.'s start displaying)
    // X2:  0x260+(WIDTH*8), where WIDTH is like 320 or whatevs
    *psx->gp1 = 0x06C60220;

    // Vertical Display Range (on screen)
    // 0b 00000111   0000   0000000000 0000000000
    //    command | unused |    Y2    |    Y1
    // similar to above, the values are relative, but generally want:
    // Y1: 0x88 - (224/2) for NTSC, 0xA3 - (264/2) for PAL
    // Y2: 0x88 + (224/2) for NTSC, 0xA3 + (264/2) for PAL
    //*psx->gp1 = 0x0704B81E;
    *psx->gp1 = 0x07040010;

    // Display mode
    // 0-1   Horizontal Resolution 1     (0=256, 1=320, 2=512, 3=640) ;GPUSTAT.17-18
    // 2     Vertical Resolution         (0=240, 1=480, when Bit5=1)  ;GPUSTAT.19
    // 3     Video Mode                  (0=NTSC/60Hz, 1=PAL/50Hz)    ;GPUSTAT.20
    // 4     Display Area Color Depth    (0=15bit, 1=24bit)           ;GPUSTAT.21
    // 5     Vertical Interlace          (0=Off, 1=On)                ;GPUSTAT.22
    // 6     Horizontal Resolution 2     (0=256/320/512/640, 1=368)   ;GPUSTAT.16
    // 7     "Reverseflag"               (0=Normal, 1=Distorted)      ;GPUSTAT.14
    // 8-23  Not used (zero)
    *psx->gp1 = 0x08000001; // 320x240 resolution, NTSC, non interlaced

    // note gp0 now
    // draw mode
    *psx->gp0 = 0xE10006Cf;

    // clip start command 0xE3
    *psx->gp0 = 0xE3000000;

    // clip end command 0xE4
    *psx->gp0 = 0xE407429F;

    // draw offset
    *psx->gp0 = 0xE5000000;

    // note back to m_gp1
    // enable display
    *psx->gp1 = 0x03000000;

    printf("Exiting initgpu\n");
}

void psx_clear_bg(PSX *psx, uint32_t color)
{
    *psx->gp0 = (0x02 << 24) | color; // command = 0x02, remaining bits = color
    *psx->gp0 = 0x00000000; // top left - 0,0
    *psx->gp0 = 0x00FF0140; // height,width - 240,320
}
