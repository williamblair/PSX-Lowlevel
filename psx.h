/*
 * psx.h
 */

#include <stdio.h>
#include <inttypes.h>

#ifndef PSX_H_INCLUDED
#define PSX_H_INCLUDED

#define GP0 0x1F801810
#define GP1 0x1F801814


#define IRQ_STAT  0x1F801070 // interrupt status register
#define IRQ_MASK  0x1F801074 // interrupt mask register

// interrupt types
#define IRQ_VBLANK 0x001

typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;

typedef struct {
    
    /* Points */
    uint16_t x;
    uint16_t y;
    
    /* Color */
    uint32_t color;
    
} Vertex;

typedef struct {
    
    Vertex v1;
    Vertex v2;
    Vertex v3;
    
} Triangle;

typedef struct {
    
    vu32 *gp0;
    vu32 *gp1;
    
    vu16 *irq_stat;
    vu16 *irq_mask;
    
} PSX;

// waits for bit 28 of the GPUSTAT register to be 1, indicating not busy
void psx_wait_gpu(PSX *psx);

// wait for bit 26 of the GPUSTAT register to be 1, indicating ready to recieve command word
void psx_wait_gpu_cmd(PSX *psx);

// waits for the vblank bit of the IRQ status register to be 0, indicating vblank done
// TODO - read more about this
void psx_wait_vblank(PSX *psx);

// initializes the gpu
void psx_init(PSX *psx);

// draws a rectangle filling the screen with the given color
// color should be in 24bit 0xBbGgRr format
void psx_clear_bg(PSX *psx, uint32_t color);

////////////////////////////////////////////////////////////////////////
/******************** Render Polygon Commands *************************/
////////////////////////////////////////////////////////////////////////
void psx_draw_triangle(PSX *psx, Triangle *t);

#endif // PSX_H_INCLUDED
