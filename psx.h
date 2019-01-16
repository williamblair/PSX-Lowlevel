/*
 * psx.h
 */

#include <stdio.h>
#include <inttypes.h>

#ifndef PSX_H_INCLUDED
#define PSX_H_INCLUDED

#define GP0 0x1F801810
#define GP1 0x1F801814

typedef volatile uint32_t vu32;

typedef struct {
    
    vu32 *gp0;
    vu32 *gp1;
    
} PSX;

// waits for bit 28 of the GPUSTAT register to be 1, indicating not busy
void psx_wait_gpu(PSX *psx);

// initializes the gpu
void psx_init(PSX *psx);

// draws a rectangle filling the screen with the given color
// color should be in 24bit 0xBbGgRr format
void psx_clear_bg(PSX *psx, uint32_t color);

#endif // PSX_H_INCLUDED
