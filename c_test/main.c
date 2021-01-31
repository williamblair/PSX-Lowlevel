/*
 * main.c
 */

#include <string.h>

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef int            int32_t;

#define GP0 0x1F801810 // GPU command ports 0 and 1
#define GP1 0x1F801814


// GP0 commands registers
#define GP0_MODE     0xE1     // draw mode setting
#define GP0_TOPLEFT  0xE3     // drawing top left X,Y
#define GP0_B_RIGHT  0xE4     // drawing bottom right X,Y
#define GP0_OFFSET   0xE5     // drawing area offset X,Y

// GP0 drawing commands
#define GP0_FILLRECT  0x02

// GP1 commands registers
#define GP1_RESET   0x00     // gpu display control 0x00 - reset GPU
#define GP1_ENABLE  0x03     // gpu display contol 0x03 - display enable
#define GP1_HRANGE  0x06     // gpu display control 0x06 - horizontal display range
#define GP1_VRANGE  0x07     // gpu display control 0x06 - vertical display range
#define GP1_MODE    0x08     // gpu display control 0x08 - display mode

// GP1_MODE parameters
#define H_320  0x01
#define V_240  0x00
#define BPP_15 0x00
#define NTSC   0x00

#define RGB(r,g,b) (((b)<<16) | ((g)<<8) | (r))

static inline void WriteRegWord(uint32_t reg, uint32_t val)
{
    *((volatile uint32_t*)reg) = val;
}

static inline void WriteGP0(uint8_t command, uint32_t param)
{
    WriteRegWord(GP0, (command<<24)+(param & 0xFFFFFF));
}

static inline void WriteGP1(uint8_t command, uint32_t param)
{
    WriteRegWord(GP1, (command<<24)+(param & 0xFFFFFF));
}

static inline void Init()
{
    // Init GPU and display mode
    WriteGP1( GP1_RESET,  0x00 ); // Reset GPU
    WriteGP1( GP1_ENABLE, 0x00 ); // Enable Display
    WriteGP1( GP1_MODE,   (H_320+V_240+BPP_15+NTSC) );// set GPU mode 320x240, 15BPP, NTSC
    WriteGP1( GP1_HRANGE, 0xC60260 ); // horizontal display range 608..3168
    WriteGP1( GP1_VRANGE, 0x042018 ); //# vertical display range 24..264
    

    // Set GPU drawing area
    WriteGP0( GP0_MODE,      0x000400 ); // Drawing to display area is allowed, bit 10
    WriteGP0( GP0_TOPLEFT,   0x000000 ); // Top left draw area X,Y = 0,0
    WriteGP0( GP0_B_RIGHT,   0x03BD3F ); // Bottom right draw area X,Y = 319,239
    WriteGP0( GP0_OFFSET,    0x000000 ); // Drawing offset X,Y = 0,0
}

static inline void FillRect( uint16_t x,
                             uint16_t y,
                             uint16_t width,
                             uint16_t height,
                             uint32_t color )
{
    WriteGP0( GP0_FILLRECT, color );
    WriteRegWord( GP0, (y << 16)+(x & 0xFFFF) );
    WriteRegWord( GP0, (height << 16)+(width & 0xFFFF) );
}

static inline void DrawChar( uint16_t x,
                             uint16_t y,
                             uint16_t fontWidth,
                             uint16_t fontHeight,
                             uint8_t* fontTexture,
                             char letter )
{
    WriteGP0( 0xA0, 0x000000 ); // copy rectangle from CPU to VRAM
    WriteRegWord( GP0, (y << 16)+(x & 0xFFFF) ); // X and Y location
    WriteRegWord( GP0, (fontHeight << 16)+(fontWidth & 0xFFFF) ); // width and height

    // calculate texture area to copy based on the letter
    uint32_t letterIndex = (uint32_t)(letter);
    int32_t amountToCopy = (fontWidth*fontHeight / 2); // size of texture memory to copy in words
    letterIndex *= 128;
    uint32_t* texVal = (uint32_t*)&fontTexture[letterIndex];
    // Copy the data one word at a time
    while ( amountToCopy > 0 )
    {
        WriteRegWord( GP0, *texVal );
        ++texVal;
        --amountToCopy;
    }
}
static inline void DrawString( uint16_t x,
                               uint16_t y,
                               uint16_t fontWidth,
                               uint16_t fontHeight,
                               uint8_t* fontTexture,
                               char* string )
{
    uint32_t i;
    uint32_t len = strlen( string );
    for (i = 0; i < len; i++)
    {
        DrawChar( x, y, fontWidth, fontHeight, fontTexture, *string );
        string++;
        x += fontWidth;
    }
}

#include "FontRed8x8.h"

int main(void)
{
    Init();
    FillRect( 0,0, // x,y
              319,239, // width, height
              RGB(100, 100, 100) ); // color

    DrawString( 10,10,
                8,8,
                FontRed,
                "Hello World" );

    while (1)
    {
    }
    
    return 0;
}


