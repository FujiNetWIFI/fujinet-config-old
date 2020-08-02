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
#include "input.h"
#include "bar.h"
#include "keys-reference.h"

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

  keys_reference_diskulator_hosts_hosts();
  
  bar_show(2);
}

/**
 * Diskulator interactive - hosts
 */
void diskulator_hosts_hosts(Context *context, SubState *new_substate)
{
  unsigned char k;
  unsigned char i=0;

  while (*new_substate==HOSTS)
    {
      k=input_handle_key();
      input_handle_nav_keys(k,2,8,&i);
      
      switch(k)
	{
	case 'D':
	case 'd':
	  *new_substate = DEVICES;
	  bar_show(i+13);
	  break;
	case 'C':
	case 'c':
	  context->state=DISKULATOR_INFO;
	  *new_substate=DONE;
	  break;
	}
    }
}

/**
 * Diskulator interactive - device slots
 */
void diskulator_hosts_devices(Context *context, SubState *new_substate)
{
  unsigned char k;
  unsigned char i=0;

  while (*new_substate==DEVICES)
    {
      k=input_handle_key();
      input_handle_nav_keys(k,13,8,&i);
      
      switch(k)
	{
	case 'H':
	case 'h':
	  *new_substate = HOSTS;
	  bar_show(i+2);
	  break;
	}
    }
}

/**
 * Connect wifi State
 */
State diskulator_hosts(Context *context)
{
  SubState ss=HOSTS;
    
  diskulator_hosts_setup(&hs,&ds);
  
  while (ss != DONE)
    {
      switch(ss)
      	{
      	case HOSTS:
      	  diskulator_hosts_hosts(context,&ss);
      	  break;
      	case DEVICES:
      	  diskulator_hosts_devices(context,&ss);
      	  break;
      	}
    }
  
  return context->state;
}
