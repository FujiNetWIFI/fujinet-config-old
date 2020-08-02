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

#define ORIGIN_HOST_SLOTS 2
#define ORIGIN_DEVICE_SLOTS 13

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
 * Handle jump keys (1-8, shift 1-8)
 */
void diskulator_hosts_handle_jump_keys(unsigned char k,unsigned char *i, SubState *new_substate)
{
  switch(k)
    {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
      *i=k-'1';
      *new_substate=DEVICES;
      bar_show((*i)+ORIGIN_DEVICE_SLOTS);
      keys_reference_diskulator_hosts_devices();
      break;
    case '!':
    case '"':
    case '#':
    case '$':
    case '%':
    case '&':
    case '\'':
    case '@':
      if (k=='@')
	*i=7;
      else
	*i=k-'!';
      *new_substate=HOSTS;
      bar_show((*i)+ORIGIN_HOST_SLOTS);
      keys_reference_diskulator_hosts_hosts();
      break;
    }
}

/**
 * Handle nav keys depending on sub-state (HOSTS or DEVICES)
 */
void diskulator_hosts_handle_nav_keys(unsigned char k, unsigned char *i, SubState *new_substate)
{
  unsigned char o;
  
  if (*new_substate==DEVICES)
    o=ORIGIN_DEVICE_SLOTS;
  else
    o=ORIGIN_HOST_SLOTS;

  input_handle_nav_keys(k,o,8,i);
}

/**
 * Edit a host slot
 */
void diskulator_hosts_edit_host_slot(unsigned char i)
{
  if (hs.host[i][0] == 0x00)
    {
      char tmp[2]={0,0};
      screen_clear_line(i+1);
      tmp[0]=i+0x31;
      screen_puts(2,i+1,tmp);
    }
  screen_input(4, i+1, hs.host[i]);
  if (hs.host[i][0] == 0x00)
    screen_puts(5, i+1, text_empty);
  fuji_sio_write_host_slots(&hs);
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
      if (input_handle_console_keys() == 0x03)
	{
	  *new_substate=DONE;
	  context->state = MOUNT_AND_BOOT;
	}

      k=input_handle_key();
      diskulator_hosts_handle_jump_keys(k,&i,new_substate);
      diskulator_hosts_handle_nav_keys(k,&i,new_substate);
      
      switch(k)
	{
	case 'D':
	case 'd':
	  i=0;
	  *new_substate = DEVICES;
	  keys_reference_diskulator_hosts_devices();
	  bar_show(i+ORIGIN_DEVICE_SLOTS);
	  break;
	case 'C':
	case 'c':
	  context->state=DISKULATOR_INFO;
	  *new_substate=DONE;
	  break;
	case 'E':
	case 'e':
	  diskulator_hosts_edit_host_slot(i);
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
      if (input_handle_console_keys() == 0x03)
	{
	  *new_substate=DONE;
	  context->state = MOUNT_AND_BOOT;
	}
      k=input_handle_key();
      diskulator_hosts_handle_jump_keys(k,&i,new_substate);
      diskulator_hosts_handle_nav_keys(k,&i,new_substate);
      
      switch(k)
	{
	case 'H':
	case 'h':
	  i=0;
	  *new_substate = HOSTS;
	  keys_reference_diskulator_hosts_hosts();
	  bar_show(i+ORIGIN_HOST_SLOTS);
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
