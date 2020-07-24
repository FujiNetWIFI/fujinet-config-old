/**
 * CONFIG - Diskulator Hosts/deviceslots
 */

#include <atari.h>
#include <stdbool.h>
#include <peekpoke.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "diskulator_host.h"
#include "screen.h"
#include "bar.h"
#include "fuji_typedefs.h"
#include "fuji_sio.h"
#include "die.h"
#include "color.h"
#include "info.h"

static HostSlots hostSlots;
static DeviceSlots deviceSlots;

/**
 * Display common keys
 */
void diskulator_host_display_common_keys(void)
{
  screen_puts(0, 21, "    \xD9\x91\x8D\x98\x19"
	      "Drives \xD9\xDC\x91\x8D\x98\x19"
	      "Hosts\xD9\xA3\x19"
	      "Config");
}

/**
 * Display hosts mode keys
 */
void diskulator_host_display_hosts_keys(void)
{
  screen_puts(0, 20, "\xD9\xB2\xA5\xB4\xB5\xB2\xAE\x19Pick\xD9\xA5\x19"
	      "Edit\xD9\xA4\x19"
	      "Drives\xD9\xAF\xB0\xB4\xA9\xAF\xAE\x19"
	      "Boot");
}

/**
 * Display devices mode keys
 */
void diskulator_host_display_devices_keys(void)
{
  screen_puts(0, 20, "        \xD9\xA5\x19"
	      "Eject\xD9\xA8\x19Hosts\xD9\xAE\x19New          ");
}

/**
 * Display device slots
 */
void diskulator_host_display_device_slots(void)
{
  unsigned char i;
  
  // Display Device Slots
  fuji_sio_read_device_slots(&deviceSlots);

  if (OS.dcb.dstats != 0x01)
    {
      screen_puts(21,0,"COULD NOT GET DEVICES!");
      die();
    }
  
  // Display drive slots
  for (i = 0; i < 8; i++)
    {
      unsigned char d[6];
      
      d[1] = 0x20;
      d[2] = 0x31 + i;
      d[4] = 0x20;
      d[5] = 0x00;
      
      if (deviceSlots.slot[i].file[0] != 0x00)
        {
	  d[0] = deviceSlots.slot[i].hostSlot + 0x31;
	  d[3] = (deviceSlots.slot[i].mode == 0x02 ? 'W' : 'R');
        }
      else
        {
	  d[0] = 0x20;
	  d[3] = 0x20;
        }
      
      screen_puts(0, i + 11, d);
      screen_puts(5, i + 11, deviceSlots.slot[i].file[0] != 0x00 ? deviceSlots.slot[i].file : "Empty");
    }
}

/**
 * Display host slots
 */
void diskulator_host_display_host_slots(void)
{
  unsigned char i;
  
  fuji_sio_read_host_slots(&hostSlots);

  if (OS.dcb.dstats != 0x01)
    {
      screen_puts(21, 0, "COULD NOT GET HOSTS!");
      die();
    }
  
  // Display host slots
  for (i = 0; i < 8; i++)
    {
      unsigned char n = i + 1;
      unsigned char nc[2];
      
      utoa(n, nc, 10);
      screen_puts(2, i + 1, nc);
      
      if (hostSlots.host[i][0] != 0x00)
	screen_puts(5, i + 1, hostSlots.host[i]);
      else
	screen_puts(5, i + 1, "Empty");
    }
}

/**
 * Diskulator Screen setup
 */
void diskulator_host_screen_setup(void)
{
  screen_clear();
  bar_clear();

  POKE(0x60F,6);
  POKE(0x610,6);
  POKE(0x61B,2);
  POKE(0x61C,2);

  screen_puts(3,0,"TNFS HOST LIST");
  screen_puts(24,9,"DRIVE SLOTS");
}

/**
 * Check and act on OPTION key
 */
void diskulator_host_option_key(void)
{
  if (GTIA_READ.consol == 0x03)
    {
      fuji_sio_mount_all_hosts(&deviceSlots, &hostSlots);
      fuji_sio_mount_all_devices(&deviceSlots);
      cold_boot();
    }  
}

/**
 * Cursor up for either hosts or devices
 */
void diskulator_host_cursor_up(unsigned char* i)
{
  if (*i > 0)
    *i--;
}

/**
 * Cursor down for either hosts or devices
 */
void diskulator_host_cursor_down(unsigned char* i)
{
  if (*i < 7)
    *i++;
}

/**
 * Process key for hosts or devices
 */
char diskulator_host_process_key(void)
{
  if (kbhit())
    return cgetc();
}

/**
 * Process common (e.g. direction) keys
 */
void diskulator_host_handle_common_keys(unsigned char k, unsigned char* i, HostMode* mode)
{
  switch(k)
    {
    case 0x1C:
    case '-':
      diskulator_host_cursor_up(i);
      break;
    case 0x1D:
    case '=':
      diskulator_host_cursor_down(i);
      break;
    case '_':
      color_luminanceIncrease();
      break;
    case '|':
      color_luminanceDecrease();
      break;
    case '\\':
      color_hueDecrease();
      break;
    case '^':
      color_hueIncrease();
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
      *i = k - '1'; // fall through to devices.
    case 'D':
    case 'd':
      *mode = DEVICES;
      break;
    case '!':
    case '"':
    case '#':
    case '$':
    case '%':
    case '&':
    case '\'':
    case '@':
      *i = k - '1'; // fall through to hosts.
    case 'H':
    case 'h':
      *mode = HOSTS;
      break;
    case 'C':
    case 'c':
      *mode = CONFIG;
      break;
    }
}

/**
 * Edit host slot
 */
void diskulator_host_edit_host_slot(unsigned char i, HostSlots* hostSlots)
{
  if (hostSlots->host[i][0] == 0x00)
    screen_puts(3, i + 1, "                                    ");

  screen_input(4, i + 1, hostSlots->host[i]);
  
  if (hostSlots->host[i][0] == 0x00)
    screen_puts(5, i + 1, "Empty");
  fuji_sio_write_host_slots(hostSlots);
}

/**
 * Handle keys for hosts pane
 */
void diskulator_host_handle_hosts_keys(unsigned char k, unsigned char i, HostSlots* hostSlots, HostMode* mode)
{
  switch(k)
    {
    case 'E':
    case 'e':
      diskulator_host_edit_host_slot(i,hostSlots);
      break;
    case 0x9B: // RETURN
      *mode = SELECTED;
      break;
    }
}

/**
 * Handle host slots mode
 */
void diskulator_host_mode_host_slots(unsigned char* i, HostMode* mode)
{
  char k;
  
  bar_clear();
  bar_show(*i+2);

  while (mode==HOSTS)
    {
      diskulator_host_option_key();
      k=diskulator_host_process_key();
      diskulator_host_handle_common_keys(k,i,mode);
      diskulator_host_handle_hosts_keys(k,*i,&hostSlots,mode);
    }
  
}

/**
 * Eject device from device slot
 */
void diskulator_host_eject(unsigned char i)
{
  screen_puts(0, i + 11, " ");
  screen_puts(3, i + 11, "  Empty                              ");
  memset(deviceSlots.slot[i].file, 0, sizeof(deviceSlots.slot[i].file));
  deviceSlots.slot[i].hostSlot = 0xFF;
  fuji_sio_write_device_slots(&deviceSlots);
}

/**
 * Handle keys for device slots
 */
void diskulator_host_handle_drives_keys(unsigned char k, unsigned char i)
{
  switch(k)
    {
    case 'E':
    case 'e':
      diskulator_host_eject(i);
      break;
    }
}

/**
 * Handle device slots mode
 */
void diskulator_host_mode_device_slots(unsigned char* i, HostMode* mode)
{
  char k;

  bar_clear();
  bar_show(*i+13);

  while (*mode==DEVICES)
    {
      diskulator_host_option_key();
      k=diskulator_host_process_key();
      diskulator_host_handle_common_keys(k,i,mode);
      diskulator_host_handle_drives_keys(k,*i);
    }
}

/**
 * Setup
 */
void diskulator_host_setup(HostMode mode)
{
  diskulator_host_screen_setup();
  diskulator_host_display_host_slots();
  diskulator_host_display_device_slots();
  diskulator_host_display_common_keys();

  switch (mode)
    {
    case HOSTS:
      diskulator_host_display_hosts_keys();
      break;
    case DEVICES:
      diskulator_host_display_devices_keys();
      break;
    }
}

/**
 * Diskulator hosts/deviceslots screen.
 */
bool diskulator_host(unsigned char* selected_host)
{
  HostMode mode=HOSTS;
  unsigned char hosts_i=0;
  unsigned char devices_i=0;
  
  diskulator_host_setup(mode);
  
  while (true)
    {
      switch(mode)
	{
	case HOSTS:
	  diskulator_host_mode_host_slots(&hosts_i,&mode);
	  break;
	case DEVICES:
	  diskulator_host_mode_device_slots(&devices_i,&mode);
	  break;
	case CONFIG:
	  info_run();
	  break;
	case SELECTED:
	  *selected_host=hosts_i;
	  return true;
	}
    }
}
