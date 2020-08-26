/**
 * #FUJINET CONFIG
 * Diskulator Info/Devices
 */

#define VERSION "2.5.2020-08-25"

#include "diskulator_info.h"
#include "fuji_typedefs.h"
#include "fuji_sio.h"
#include "screen.h"
#include "error.h"
#include "input.h"

/**
 * Connect wifi State
 */
State diskulator_info(Context *context)
{
  State new_state = DISKULATOR_HOSTS;
  AdapterConfig ac;
  char k=0;
  
  screen_dlist_diskulator_info();

  fuji_sio_read_adapter_config(&ac);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_ADAPTER_CONFIG);

  screen_puts(0, 4, "  #FUJINET  CONFIG  ");
  screen_puts(7, 15, "\xD9\xA3\x19RECONNECT \xD9\xB3\x19" "CHANGE SSID");
  screen_puts(9, 16, "Any other key to return");
  screen_puts(5, 5, "      SSID:");
  screen_puts(5, 6, "  Hostname:");
  screen_puts(5, 7, "IP Address:");
  screen_puts(5, 8, "   Gateway:");
  screen_puts(5, 9, "       DNS:");
  screen_puts(5, 10, "   Netmask:");
  screen_puts(5, 11, "       MAC:");
  screen_puts(5, 12, "     BSSID:");
  screen_puts(5, 13, "   Version:" VERSION);

  screen_puts(17, 5, ac.ssid);
  screen_puts(17, 6, ac.hostname);
  screen_print_ip(17, 7, ac.localIP);
  screen_print_ip(17, 8, ac.gateway);
  screen_print_ip(17, 9, ac.dnsIP);
  screen_print_ip(17, 10, ac.netmask);
  screen_print_mac(17, 11, ac.macAddress);
  screen_print_mac(17, 12, ac.bssid);

  while (k==0)
    k=input_handle_key();

  switch(k)
    {
    case 'C':
    case 'c':
      new_state = CONNECT_WIFI;
      break;
    case 'S':
    case 's':
      new_state = SET_WIFI;
      break;
    }
  
  return new_state;
}
