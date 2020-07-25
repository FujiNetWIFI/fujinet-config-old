/**
 * FujiNet Configurator
 *
 * Function to die, wait for keypress and then do coldstart
 */

#include <conio.h>

/**
 * Do cold start
 */
void cold_boot(void)
{
  asm("jmp $E477");
}

/**
 * Stop, wait for keypress, then coldstart
 */
void die(void)
{
  while (!kbhit()) { }
  cold_boot();
}

