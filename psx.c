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

void psx_wait_gpu_cmd(PSX *psx)
{
    uint8_t status;
    
    do {
        
        // get bit 28 from the status register
        status = ((*psx->gp1) >> 26) & 1;
        
    } while (!status); // bit 26 = 0 -> not ready to receive command word
}

// taken from ARM9 psxdev - https://github.com/ARM9/psxdev
void psx_wait_vblank(PSX *psx)
{
    // 0 == no irq
    while((*psx->irq_stat & IRQ_VBLANK) == 0);
    
    // acknowledge the interrupt (write a zero to the vblank bit)
    *psx->irq_stat &= ~IRQ_VBLANK;
    
    printf("Exiting wait vblank\n");
}

void psx_init(PSX *psx)
{
    // Set pointers
    psx->gp0 = (vu32*)GP0;
    psx->gp1 = (vu32*)GP1;
    psx->irq_mask = (vu16*)I_MASK;
    psx->irq_stat = (vu16*)I_STAT;
    
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
    // Draw Mode setting (aka "Texpage")
    // 0-3   Texture page X Base   (N*64) (ie. in 64-halfword steps)    ;GPUSTAT.0-3
    // 4     Texture page Y Base   (N*256) (ie. 0 or 256)               ;GPUSTAT.4
    // 5-6   Semi Transparency     (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)   ;GPUSTAT.5-6
    // 7-8   Texture page colors   (0=4bit, 1=8bit, 2=15bit, 3=Reserved);GPUSTAT.7-8
    // 9     Dither 24bit to 15bit (0=Off/strip LSBs, 1=Dither Enabled) ;GPUSTAT.9
    // 10    Drawing to display area (0=Prohibited, 1=Allowed)          ;GPUSTAT.10
    // 11    Texture Disable (0=Normal, 1=Disable if GP1(09h).Bit0=1)   ;GPUSTAT.15
    //         (Above might be chipselect for (absent) second VRAM chip?)
    // 12    Textured Rectangle X-Flip   (BIOS does set this bit on power-up...?)
    // 13    Textured Rectangle Y-Flip   (BIOS does set it equal to GPUSTAT.13...?)
    // 14-23 Not used (should be 0)
    *psx->gp0 = 0xE10006CF;

    // Set drawing area top left (X1,Y1)
    *psx->gp0 = 0xE3000000;

    // Set drawing area bottom right (X2,Y2)
    // Render commands GP0(0x20..0x7F) automatically clip anything 
    //   outside of this region
    *psx->gp0 = 0xE407429F;

    // Set drawing offset
    *psx->gp0 = 0xE5000000;

    // note back to gp1
    // Display Enable
    *psx->gp1 = 0x03000000;

    printf("Exiting initgpu\n");
}

void psx_clear_bg(PSX *psx, uint32_t color)
{
    // draw rectangle in vram command
    *psx->gp0 = (0x02 << 24) | color; // command = 0x02, remaining bits = color
    *psx->gp0 = 0x00000000; // top left - 0,0
    *psx->gp0 = 0x00FF0140; // height,width - 240,320
}

////////////////////////////////////////////////////////////////////////
/******************** Render Polygon Commands *************************/
////////////////////////////////////////////////////////////////////////
void psx_draw_triangle(PSX *psx, Triangle *t)
{
    psx_wait_gpu_cmd(psx);
    
    // command 0x30 - shaded three point polygon, opaque + first color
    *psx->gp0 = (0x30 << 24) | t->v1.color;
    
    // first vertex
    *psx->gp0 = (t->v1.y << 16) | t->v1.x;
    
    // second color
    *psx->gp0 = t->v2.color;
    
    // second vertex
    *psx->gp0 = (t->v2.y << 16) | t->v2.x;
    
    // third color
    *psx->gp0 = t->v3.color;
    
    // third vertex
    *psx->gp0 = (t->v3.y << 16) | t->v3.x;
}

void psx_draw_rectangle(PSX *psx, Rectangle *r)
{
    psx_wait_gpu_cmd(psx);
    
    // command 0x38 - shaded four point polygon, opaque + first color
    *psx->gp0 = (0x38 << 24) | r->v1.color;
    
    // first vertex
    *psx->gp0 = (r->v1.y << 16) | r->v1.x;
    
    // second color
    *psx->gp0 = r->v2.color;
    
    // second vertex
    *psx->gp0 = (r->v2.y << 16) | r->v2.x;
    
    // third color
    *psx->gp0 = r->v3.color;
    
    // third vertex
    *psx->gp0 = (r->v3.y << 16) | r->v3.x;
    
    // fourth color
    *psx->gp0 = r->v4.color;
    
    // fourth vertex
    *psx->gp0 = (r->v4.y << 16) | r->v4.x;
}






