/**
 * FujiNet Configurator
 *
 * Functions to display a selection bar
 */

#include <atari.h>
#include <string.h>
#include "bar.h"

unsigned char* bar_pmbase=(unsigned char *)0x7C00;

/**
 * Clear bar from screen
 */
void bar_clear(void)
{
  memset(bar_pmbase,0,1024);
}

/**
 * Show bar at Y position
 */
void bar_show(unsigned char y)
{
  unsigned char scrpos=((y<<2)+16)+384;
  unsigned char pmgcount=5;

  bar_clear();
  
  do {
    bar_pmbase[scrpos+0]=0xFF;
    bar_pmbase[scrpos+1]=0xFF;
    bar_pmbase[scrpos+2]=0xFF;
    bar_pmbase[scrpos+3]=0xFF;
    scrpos = scrpos + 0x80;
    pmgcount--;
  } while (pmgcount > 0);

}

/**
 * Set bar color
 */
void bar_set_color(unsigned char c)
{
  OS.pcolr0=OS.pcolr1=OS.pcolr2=OS.pcolr3=c;
}
