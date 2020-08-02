/**
 * #FUJINET CONFIG
 *
 * Routines to display available keys for a mode
 */

#include "keys-reference.h"
#include "screen.h"

char text_mode_hosts[]=
  "\xD9\xB2\xA5\xB4\xB5\xB2\xAE\x19Pick\xD9\xA5\x19"
  "Edit\xD9\xA4\x19"
  "Drives\xD9\xAF\xB0\xB4\xA9\xAF\xAE\x19"
  "Boot"
  "    \xD9\x91\x8D\x98\x19"
  "Drives \xD9\xDC\x91\x8D\x98\x19"
  "Hosts\xD9\xA3\x19"
  "Config";

/**
 * Routine to display keys output
 */
void keys_display(char* text)
{
  screen_puts(0,20,text);
}

/**
 * Keys used in Diskulator_hosts hosts mode
 */
void keys_reference_diskulator_hosts_hosts(void)
{
  keys_display(text_mode_hosts);
}
