/**
 * FujiNet Configuration Program
 */

#include <atari.h>
#include <stdbool.h>
#include <string.h>
#include <peekpoke.h>
#include "bar.h"
#include "screen.h"
#include "font.h"
#include "state.h"
#include "fuji_typedefs.h"
#include "fuji_sio.h"
#include "error.h"
#include "color.h"

Context context;

/**
 * Is device configured?
 */
State configured(Context *context)
{
  NetConfig netConfig;
  unsigned char status;
  
  fuji_sio_read_net_config(&netConfig);

    // WiFi not configured or SELECT to override.
  if ((GTIA_READ.consol == 5) || netConfig.ssid[0] == '\0')
    {
      return SET_WIFI;
    }
  else if (netConfig.ssid[0] != '\0')
    {
      if (fuji_sio_get_wifi_status(&status) != 1)
	error_fatal(ERROR_READING_WIFI_STATUS);

      if (status==3)
	{
	  context->net_connected=true;
	  bar_set_color(COLOR_SETTING_SUCCESSFUL);
	  return DISKULATOR_HOSTS;
	}
      else
        {
          return CONNECT_WIFI;
        }
    }
    
  return DISKULATOR_HOSTS;
}

/**
 * Setup the config screen
 */
void setup(Context *context)
{
  OS.noclik=0xFF;
  OS.shflok=0;
  OS.color0=0x9f;
  OS.color1=0x0f;
  OS.color2=0x90;
  OS.color4=0x90;
  OS.coldst=1;
  OS.sdmctl=0; // Turn off screen

  screen_init();

  font_init();

  bar_init();
  context_setup(context);
}

void main(void)
{
  setup(&context);

  context.net_connected = false;
  context.state = configured(&context);
  
  state(&context); // Never ends.
  asm("nop");
}
