/**
 * #FUJINET CONFIG
 * Set Wifi Network
 */

#include "set_wifi.h"
#include "screen.h"

char colon[] = ":";

/**
 * Setup 
 */
void set_wifi_setup(void)
{
  screen_puts(0,  0, "WELCOME TO #FUJINET!");
  screen_puts(0, 21, "SCANNING NETWORKS...");
  screen_puts(0,  1, "MAC Address:");
  screen_puts(15, 1, colon);
  screen_puts(18, 1, colon);
  screen_puts(21, 1, colon);
  screen_puts(24, 1, colon);
  screen_puts(27, 1, colon);
}

/**
 * Set wifi State
 */
State set_wifi(Context *context)
{
  State new_state = CONNECT_WIFI;

  set_wifi_setup();
  
  return new_state;
}
