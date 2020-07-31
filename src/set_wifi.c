/**
 * #FUJINET CONFIG
 * Set Wifi Network
 */

#include <stdlib.h>
#include "set_wifi.h"
#include "screen.h"
#include "fuji_sio.h"
#include "fuji_typedefs.h"
#include "error.h"

char colon[] = ":";

/**
 * Display MAC address
 */
void set_wifi_display_mac_address(AdapterConfig *adapterConfig)
{
  char mactmp[3];
  unsigned char i=0;
  unsigned char x=13;

  for (i=0;i<6;i++)
    {
      itoa(adapterConfig->macAddress[i],mactmp,16);
      screen_puts(x,1,mactmp);
      x+=3;
    }
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
 * Print SSID
 */
void set_wifi_print_ssid(SSIDInfo *s, unsigned char i)
{
  screen_puts(2, i+3, (char *)s->ssid);
}

/**
 * Print RSSI (strength)
 */
void set_wifi_print_rssi(SSIDInfo *s, unsigned char i)
{
  char out[4] = {0x20, 0x20, 0x20, 0x00};
  
  if (s->rssi > -40)
    {
      out[0] = 0x01;
      out[1] = 0x02;
      out[2] = 0x03;
    }
  else if (s->rssi > -60)
    {
      out[0] = 0x01;
      out[1] = 0x02;
    }
  else
    {
      out[0] = 0x01;
    }
  
  screen_puts(35, i + 3, out);
}

/**
 * Scan and display networks
 */
void set_wifi_scan_networks(void)
{
  unsigned char i;
  unsigned char numNetworks = fuji_sio_do_scan();
 
  if (fuji_sio_error())
    error_fatal(ERROR_SCANNING_NETWORKS);

  for (i=0;i<numNetworks;i++)
    {
      SSIDInfo s;
      
      fuji_sio_scan_result(i,&s);

      if (fuji_sio_error())
	error_fatal(ERROR_READING_SCAN_RESULT);

      set_wifi_print_ssid(&s,i);
      set_wifi_print_rssi(&s,i);
    }
}

/**
 * Set wifi State
 */
State set_wifi(Context *context)
{
  AdapterConfig adapterConfig;
  State new_state = CONNECT_WIFI;

  set_wifi_setup();
  
  fuji_sio_read_adapter_config(&adapterConfig);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_ADAPTER_CONFIG);

  set_wifi_display_mac_address(&adapterConfig);
  set_wifi_scan_networks();
  
  return new_state;
}
