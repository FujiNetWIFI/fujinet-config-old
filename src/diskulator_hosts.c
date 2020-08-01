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
#include "die.h"

char text_empty[]="Empty";

static DeviceSlots ds;
static HostSlots hs;

typedef enum _substate
  {
   HOSTS,
   DEVICES,
   DONE
  } SubState;

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
 * Display device slots
 */
void diskulator_hosts_display_device_slots(DeviceSlots *ds)
{
  unsigned char i;

  // Display device slots
  for (i = 0; i < 8; i++)
    {
      unsigned char d[6];
      
      d[1] = 0x20;
      d[2] = 0x31 + i;
      d[4] = 0x20;
      d[5] = 0x00;
      
      if (ds->slot[i].file[0] != 0x00)
        {
	  d[0] = ds->slot[i].hostSlot + 0x31;
	  d[3] = (ds->slot[i].mode == 0x02 ? 'W' : 'R');
        }
      else
        {
	  d[0] = 0x20;
	  d[3] = 0x20;
        }
      
      screen_puts(0, i + 11, d);
      screen_puts(5, i + 11, ds->slot[i].file[0] != 0x00 ? ds->slot[i].file : text_empty);
    }
}

/**
 * Diskulator hosts setup
 */
void diskulator_hosts_setup(HostSlots *hs, DeviceSlots *ds)
{
  screen_dlist_diskulator_hosts();
  
  screen_puts(3, 0, "TNFS HOST LIST");
  screen_puts(24, 9, "DRIVE SLOTS");

  fuji_sio_read_host_slots(hs);
  
  if (fuji_sio_error())
    error_fatal(ERROR_READING_HOST_SLOTS);
  
  diskulator_hosts_display_host_slots(hs);

  fuji_sio_read_device_slots(ds);
  
  if (fuji_sio_error())
    error_fatal(ERROR_READING_DEVICE_SLOTS);
  
  diskulator_hosts_display_device_slots(ds);
}

/**
 * Diskulator interactive - hosts
 */
State diskulator_hosts_hosts(Context *context)
{
}

/**
 * Diskulator interactive - device slots
 */
State diskulator_hosts_devices(Context *context)
{
}

/**
 * Connect wifi State
 */
State diskulator_hosts(Context *context)
{
  SubState ss=HOSTS;
  
  State new_state = DISKULATOR_HOSTS;
  
  diskulator_hosts_setup(&hs,&ds);
  
  while (ss != DONE)
    {
      switch(ss)
	{
	case HOSTS:
	  new_state = diskulator_hosts_hosts(context);
	  break;
	case DEVICES:
	  new_state = diskulator_hosts_devices(context);
	  break;
	}
    }
  
  return new_state;
}
