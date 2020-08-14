/**
 * #FUJINET CONFIG
 * Connect Wifi Network
 */

#include <atari.h>
#include "connect_wifi.h"
#include "screen.h"
#include "bar.h"
#include "fuji_sio.h"
#include "error.h"
#include "sio.h"
#include "color.h"
#include "fuji_typedefs.h"
#include "die.h"

/**
 * Setup
 */
void connect_wifi_setup(char* ssid)
{
  screen_dlist_wifi();
  screen_puts(0,0,"WELCOME TO #FUJINET! CONNECTING TO NET");
  screen_puts(2,2,ssid);
  bar_show(3);
}

/**
 * Wait for network connection
 */
State connect_wifi_wait_for_network(NetConfig* n, Context *context)
{
  State new_state = SET_WIFI;
  unsigned char wifiStatus=0;
  
  rtclr();

  while(true)
    {
      if ((OS.rtclok[2] & 0x3f) != 0)
	  continue;

      fuji_sio_get_wifi_status(&wifiStatus);
      
      if (fuji_sio_error())
	error_fatal(ERROR_READING_WIFI_STATUS);

      if (wifiStatus==6) // DISCONNECTED
	continue;
      else if (wifiStatus > 0)
	break;
    }

  switch(wifiStatus)
    {
    case 1:
      error(ERROR_WIFI_NO_SSID_AVAILABLE);
      break;
    case 3:
      error(ERROR_WIFI_CONNECTION_SUCCESSFUL);
      break;
    case 4:
      error(ERROR_WIFI_CONNECT_FAILED);
      break;
    case 5:
      error(ERROR_WIFI_CONNECTION_LOST);
      break;
    }

  if (wifiStatus != 3)
    bar_set_color(COLOR_SETTING_FAILED);
  else
    {
      bar_set_color(COLOR_SETTING_SUCCESSFUL);
      fuji_sio_set_ssid(true,n);
      wait_a_moment();
      new_state = DISKULATOR_HOSTS;
    }

  return new_state;
}

/**
 * Connect wifi State
 */
State connect_wifi(Context *context)
{
  NetConfig n;
  
  fuji_sio_read_net_config(&n);

  if (fuji_sio_error())
    error_fatal(ERROR_READING_NET_CONFIG);
  
  connect_wifi_setup(n.ssid);
  fuji_sio_set_ssid(false,&n);

  if (fuji_sio_error())
    error_fatal(ERROR_SETTING_SSID);

  return connect_wifi_wait_for_network(&n,context);
}
