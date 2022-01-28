/**
 * FujiNet Configuration Program
 *
 * Font functions
 */
#include <atari.h>
#include <stdlib.h>
#include <string.h>
#include "font.h"



unsigned char fontPatch[48]={
			 0,0,0,0,0,0,3,51,
			 0,0,3,3,51,51,51,51,
			 48,48,48,48,48,48,48,48,
			 0,120,135,255,255,255,255,0,
			 0x00, 0x78, 0x87, 0xff, 0xff, 0xff, 0xff, 0x00,
			 0,48,120,252,48,48,48,0			 
};

void font_init()
{
  // Copy ROM font
  memcpy((unsigned char *)FONT_MEMORY,(unsigned char *)0xE000,1024);

  // And patch it.
  memcpy(FONT_MEMORY+520,&fontPatch,sizeof(fontPatch));

  OS.chbas=FONT_MEMORY>>8; // use the charset

}
