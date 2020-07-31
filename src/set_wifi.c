/**
 * #FUJINET CONFIG
 * Set Wifi Network
 */

#include "set_wifi.h"
#include "screen.h"
#include "fuji_sio.h"
#include "fuji_typedefs.h"
#include "error.h"

char colon[] = ":";

/**
 * Display MAC address
 */
void set_wifi_display_mac_address(NetConfig *netConfig)
{
  
}

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
  NetConfig netConfig;
  State new_state = CONNECT_WIFI;

  set_wifi_setup();
  fuji_sio_read_config(&netConfig);

  if (fuji_sio_error())
    error_fatal(ERROR_READING_CONFIG);
  
  return new_state;
}
