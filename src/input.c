/**
 * #FUJINET Config
 *
 * Common input routines
 */

#include <atari.h>
#include <conio.h>
#include "sio.h"
#include "bar.h"

/**
 * Handle nav up
 */
void input_handle_nav_up(unsigned char* i)
{
  if (*i>0)
    (*i)--;
}

void input_handle_nav_down(unsigned char max, unsigned char* i)
{
  if (*i<max-1)
    (*i)++;
}

/**
 * Handle common nav keys
 * k is the input ATASCII key
 * i is the maximum down #
 */
void input_handle_nav_keys(char k, unsigned char pos, unsigned char max, unsigned char *i)
{
  switch(k)
    {
    case 0x1C:
    case '-':
      input_handle_nav_up(i);
      break;
    case 0x1D:
    case '=':
      input_handle_nav_down(max,i);
      break;
    }

  if (k>0)
    bar_show(pos+(*i));
}

/**
 * Handle joystick input
 */
unsigned char input_handle_joystick(void)
{
  if ((OS.stick0!=0x0F) && (OS.rtclok[2]>6))
    {
      rtclr();
      switch(OS.stick0)
	{
	case 14:
	  return 0x1C;
	case 13:
	  return 0x1D;
	}
    }
  else if (OS.strig0 == 0)
    return 0x9B;
  else
    return 0;
}

/**
 * Handle global console keys.
 */
unsigned char input_handle_console_keys(void)
{
  return GTIA_READ.consol;
}

/**
 * Get an input from keyboard/joystick
 * Returns an 'atascii key' regardless of input
 */
unsigned char input_handle_key(void)
{
  if (kbhit())
    {
      rtclr();
      return cgetc();
    }
  else
    return input_handle_joystick();
}
