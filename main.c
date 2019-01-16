/*
 * main.c
 */

#include <stdio.h>

#include "psx.h"

int main(void)
{
    PSX psx;
    psx_init(&psx);
    
    // triangle vertices
    Triangle tri = (Triangle){
        {10,  10, 0xFF0000},
        {10, 100, 0x00FF00},
        {100, 10, 0x0000FF}
    };
    
    // rectangle vertices
    // from no$psx - Four-point polygons are internally processed as two Three-point polygons, the first consisting of Vertices 1,2,3, and the second of Vertices 2,3,4.
    // so you want to zig-zag coordinates
    Rectangle rect = (Rectangle) {
        { 50, 100, 0xFF0000},
        {100, 100, 0xFF00FF},
        { 50, 200, 0x00FF00},
        {100, 200, 0x0000FF}
    };
    
    for(;;)
    {
        psx_clear_bg(&psx, 0x888800);
        
        psx_draw_triangle(&psx, &tri);
        
        psx_draw_rectangle(&psx, &rect);
        
        psx_wait_vblank(&psx);
    }

    return 0;
}


