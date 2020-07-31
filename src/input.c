/**
 * #FUJINET Config
 *
 * Common input routines
 */

#include <atari.h>
#include <conio.h>

/**
 * Get an input from keyboard/joystick
 * Returns an 'atascii key' regardless of input
 */
unsigned char input_handle_key(void)
{
  if (kbhit())
    return cgetc();

  // TODO: Process joystick input.
}
