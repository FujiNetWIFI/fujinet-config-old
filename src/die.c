/**
 * FujiNet Configurator
 *
 * Function to die, wait for keypress and then do coldstart
 */

#include <atari.h>
#include <conio.h>
#include "sio.h"

/**
 * Stop, wait for keypress, then coldstart
 */
void die(void)
{
  while (!kbhit()) { }
  asm("jmp $E477");
}

/**
 * Wait a moment.
 */
void wait_a_moment(void)
{
  rtclr();
  while (OS.rtclok[2]<128) { }
}
