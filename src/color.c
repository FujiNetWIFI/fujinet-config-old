/**
 * CONFIG - color functions
 */

#include <atari.h>
#include "color.h"

unsigned char backgroundLum;
unsigned char backgroundHue;

void updateColors(void) 
{
    OS.color4 = OS.color2 = (backgroundHue << 4) | (backgroundLum < 1);
}

void color_luminanceIncrease(void)
{
    backgroundLum = ++backgroundLum & 7;
    updateColors();
}

void color_luminanceDecrease(void)
{
    backgroundLum = --backgroundLum & 7;
    updateColors();
}

void color_hueDecrease(void)
{
    backgroundHue = --backgroundHue & 15;
    updateColors();
}

void color_hueIncrease(void)
{
    backgroundHue = ++backgroundHue & 15;
    updateColors();
}
