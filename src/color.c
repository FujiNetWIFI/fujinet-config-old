/**
 * CONFIG - color functions
 */

#include <atari.h>
#include <peekpoke.h>
#include "color.h"

int backgroundLum;
int backgroundHue;

void luminanceIncrease(void)
{
    backgroundLum++;

    if (backgroundLum > 7)
        backgroundLum = 0;

    POKE(710, _gtia_mkcolor(backgroundHue, backgroundLum));
    POKE(712, _gtia_mkcolor(backgroundHue, backgroundLum));
}

void luminanceDecrease(void)
{
    backgroundLum--;

    if (backgroundLum < 0)
        backgroundLum = 7;

    POKE(710, _gtia_mkcolor(backgroundHue, backgroundLum));
    POKE(712, _gtia_mkcolor(backgroundHue, backgroundLum));
}

void hueDecrease(void)
{
    backgroundHue--;

    if (backgroundHue < 0)
        backgroundHue = 15;

    POKE(710, _gtia_mkcolor(backgroundHue, backgroundLum));
    POKE(712, _gtia_mkcolor(backgroundHue, backgroundLum));
}

void hueIncrease(void)
{
    backgroundHue++;

    if (backgroundHue > 15)
        backgroundHue = 0;

    POKE(710, _gtia_mkcolor(backgroundHue, backgroundLum));
    POKE(712, _gtia_mkcolor(backgroundHue, backgroundLum));
}
