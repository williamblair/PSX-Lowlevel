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
    
    for(;;)
    {
        psx_clear_bg(&psx, 0x00FFFF);
        
        psx_draw_triangle(&psx, &tri);
        
        psx_wait_vblank(&psx);
    }

    return 0;
}


