/*
 * main.c
 */

#include <stdio.h>

#include "psx.h"

int main(void)
{
    PSX psx;
    
    psx_init(&psx);
    psx_clear_bg(&psx, 0xFFFFFF);

    for(;;){}

    return 0;
}


