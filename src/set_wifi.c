/**
 * #FUJINET CONFIG
 * Set Wifi Network
 */

#include <stdlib.h>
#include <string.h>
#include <atari.h>
#include "set_wifi.h"
#include "screen.h"
#include "fuji_sio.h"
#include "fuji_typedefs.h"
#include "error.h"
#include "input.h"
#include "bar.h"

#define MAX_NETWORKS 16 // Max visible networks on screen

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
  screen_dlist_wifi();
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
 * Return # of results.
 */
unsigned char set_wifi_scan_networks(void)
{
  unsigned char i;
  unsigned char numNetworks = fuji_sio_do_scan();

  if (numNetworks > MAX_NETWORKS)
    numNetworks = MAX_NETWORKS;
  
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

  screen_clear_line((numNetworks++)+3);
  screen_puts(2,(numNetworks)+2,"<Enter a specific SSID>");

  bar_show(4); // Start here.
  
  return numNetworks;
}

/**
 * Save selection
 * selectedNetwork = index from wifi scan.
 */
State set_wifi_save_network(unsigned char selectedNetwork, unsigned char numNetworks, Context *context)
{
  SSIDInfo s;
  NetConfig n;

  memset(&s,0,sizeof(s));
  memset(&n,0,sizeof(n));
  
  if (selectedNetwork<numNetworks-1)
    {
      // Get scan result for SSID
      fuji_sio_scan_result(selectedNetwork,&s);

      if (fuji_sio_error())
	error_fatal(ERROR_READING_SCAN_RESULT);

      strcpy(n.ssid,s.ssid);
    }
  else
    {
      // Get custom SSID
      screen_puts(0,21," ENTER NETWORK NAME "
		       "  AND PRESS return  ");

      // Clear and get input.
      screen_clear_line((numNetworks-1)+3);
      screen_input(2,(numNetworks-1)+3,n.ssid);
    }

  screen_clear_line(21);
  screen_puts(3,21,"ENTER PASSWORD");
  screen_input(0,20,n.password);

  screen_puts(3,21,"SAVING NETWORK");

  fuji_sio_set_ssid(true, &n); // turns out we have to save the setting here anyway.

  if (fuji_sio_error())
    error_fatal(ERROR_SETTING_SSID);

  context->state=CONNECT_WIFI;

  return CONNECT_WIFI;  
}

/**
 * Select a network
 */
State set_wifi_select_network(unsigned char numNetworks, Context *context)
{
  State new_state;
  unsigned char k=0;
  unsigned char i=0;

  screen_puts(0,21," SELECT NET, S SKIP "
	           "   ESC TO RE-SCAN   ");
  
  while (true) // Process keys
    {
      k=input_handle_key_ucase();
      input_handle_nav_keys(k,4,numNetworks,&i);

      if ((k==KCODE_RETURN) || (k==KCODE_ESCAPE) || (k=='S'))
	break;
    }

  if (k==KCODE_RETURN) // RETURN
    {
      new_state = set_wifi_save_network(i,numNetworks,context);
    }
  else if (k==KCODE_ESCAPE) // ESC
    {
      // Loop back around again to re-scan.
      new_state = SET_WIFI;
    }
  else if (k=='S')
    {
      // Skip and go to hosts screen.
      context->net_connected = true;
      new_state = DISKULATOR_HOSTS;
    }
  
  return new_state;
}

/**
 * Set wifi State
 */
State set_wifi(Context *context)
{
  AdapterConfig adapterConfig;
  State new_state = CONNECT_WIFI;
  unsigned char numNetworks;
  
  set_wifi_setup();
  
  fuji_sio_read_adapter_config(&adapterConfig);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_ADAPTER_CONFIG);

  set_wifi_display_mac_address(&adapterConfig);
  numNetworks = set_wifi_scan_networks();

  new_state = set_wifi_select_network(numNetworks, context);

  return new_state;
}
