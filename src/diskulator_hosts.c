/**
 * #FUJINET CONFIG
 * Diskulator Hosts/Devices
 */

#include <stdlib.h>
#include "diskulator_hosts.h"
#include "screen.h"
#include "fuji_sio.h"
#include "fuji_typedefs.h"
#include "error.h"

char text_empty[]="Empty";

/**
 * Diskulator hosts setup
 */
void diskulator_hosts_setup(void)
{
  screen_dlist_diskulator_hosts();
  screen_puts(3, 0, "TNFS HOST LIST");
  screen_puts(24, 9, "DRIVE SLOTS");
}

/**
 * Display Hosts Slots
 */
void diskulator_hosts_display_host_slots(HostSlots *hs)
{
  unsigned char i;

  // Display host slots
  for (i = 0; i < 8; i++)
    {
      unsigned char n = i + 1;
      unsigned char ni[2];
      
      utoa(n, ni, 10);
      screen_puts(2, i + 1, ni);
      
      if (hs->host[i][0] != 0x00)
	screen_puts(5, i + 1, hs->host[i]);
      else
	screen_puts(5, i + 1, text_empty);
    }
}

/**
 * Connect wifi State
 */
State diskulator_hosts(Context *context)
{
  HostSlots hs;
  
  State new_state = DISKULATOR_HOSTS;

  fuji_sio_read_host_slots(&hs);

  if (fuji_sio_error())
    error_fatal(ERROR_READING_HOST_SLOTS);

  diskulator_hosts_display_host_slots(&hs);
  
  return new_state;
}
