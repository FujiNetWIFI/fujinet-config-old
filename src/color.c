/**
 * CONFIG - color functions
 */

#include <atari.h>
#include <peekpoke.h>
#include "color.h"

int backgroundLum;
int backgroundHue;

void color_luminanceIncrease(void)
{
    backgroundLum++;

    if (backgroundLum > 7)
        backgroundLum = 0;

    POKE(710, _gtia_mkcolor(backgroundHue, backgroundLum));
    POKE(712, _gtia_mkcolor(backgroundHue, backgroundLum));
}

void color_luminanceDecrease(void)
{
    backgroundLum--;

    if (backgroundLum < 0)
        backgroundLum = 7;

    POKE(710, _gtia_mkcolor(backgroundHue, backgroundLum));
    POKE(712, _gtia_mkcolor(backgroundHue, backgroundLum));
}

void color_hueDecrease(void)
{
    backgroundHue--;

    if (backgroundHue < 0)
        backgroundHue = 15;

    POKE(710, _gtia_mkcolor(backgroundHue, backgroundLum));
    POKE(712, _gtia_mkcolor(backgroundHue, backgroundLum));
}

void color_hueIncrease(void)
{
    backgroundHue++;

    if (backgroundHue > 15)
        backgroundHue = 0;

    POKE(710, _gtia_mkcolor(backgroundHue, backgroundLum));
    POKE(712, _gtia_mkcolor(backgroundHue, backgroundLum));
}
