/**
 * CONFIG - color functions
 */

#include <atari.h>
#include <peekpoke.h>
#include "color.h"

unsigned char backgroundLum;
unsigned char backgroundHue;

void updateColors(void) 
{
    OS.color2 = _gtia_mkcolor(backgroundHue, backgroundLum);
    OS.color3 = _gtia_mkcolor(backgroundHue, backgroundLum);
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
