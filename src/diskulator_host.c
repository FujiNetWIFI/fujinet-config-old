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
 * Diskulator Screen setup
 */
void diskulator_screen_setup(void)
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
 * Diskulator hosts/deviceslots screen.
 */
bool diskulator_host(unsigned char* selected_host)
{
  char tmp_str[8];
  char disk_type;
  unsigned short ns;
  unsigned short ss;
  bool ret = false;
  bool host_done = false;
  bool slot_done = true;
  char k = 0;
  unsigned char i;
  unsigned char prev_consol;
  int retval;
  
  
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
  
  // Display Device Slots
  fuji_sio_read_device_slots(&deviceSlots);
  
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
  
 rehosts:
  // reset cursor
  i = 0;
  
 rehosts_jump:
  screen_puts(0, 20, "\xD9\xB2\xA5\xB4\xB5\xB2\xAE\x19Pick\xD9\xA5\x19"
	      "Edit\xD9\xA4\x19"
	      "Drives\xD9\xAF\xB0\xB4\xA9\xAF\xAE\x19"
	      "Boot");
  screen_puts(0, 21, "    \xD9\x91\x8D\x98\x19"
	      "Drives \xD9\xDC\x91\x8D\x98\x19"
	      "Hosts\xD9\xA3\x19"
	      "Config");
  
  bar_clear();
  bar_show(i + 2);
  
  // Keyboard input in HOSTS area
  while (host_done == false)
    {
      // Quick boot
      if (GTIA_READ.consol == 0x03)
        {
	  fuji_sio_mount_all_hosts(&deviceSlots, &hostSlots);
	  fuji_sio_mount_all_devices(&deviceSlots);
	  cold_boot();
        }
      
      if (kbhit())
        {
	  k = cgetc();
	  
	  //utoa(k, zx, 10);
	  //screen_puts(0, 0, zx );
	  
	  switch (k)
            {
            case 0x1C: // ATASCII UP
            case '-':
	      if (i > 0)
		i--;
	      break;
            case 0x1D: // ATASCII DOWN
            case '=':
	      if (i < 7)
		i++;
	      break;
            case '_': // SHIFT + UP ARROW
	      color_luminanceIncrease();
	      break;
            case '|': // SHIFT + DOWN ARROW
	      color_luminanceDecrease();
	      break;
            case '\\': // SHIFT + LEFT ARROW
	      color_hueDecrease();
	      break;
            case '^': // SHIFT + RIGHT ARROW
	      color_hueIncrease();
	      break;
            case '1': // Drives 1-8
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
	      i = k - '1';
	      goto jump_to_devs;
	      break;
            case 'C': // Config
            case 'c':
	      host_done = true;
	      slot_done = true;
	      ret = false;
	      info_run();
	      break;
            case 'E': // Edit
            case 'e':
	      if (hostSlots.host[i][0] == 0x00)
		screen_puts(3, i + 1, "                                    ");
	      screen_input(4, i + 1, hostSlots.host[i]);
	      if (hostSlots.host[i][0] == 0x00)
		screen_puts(5, i + 1, "Empty");
	      fuji_sio_write_host_slots(&hostSlots);
	      break;
            case 'D': // Drives
            case 'd':
	      i = 0;
            jump_to_devs:
	      host_done = true;
	      slot_done = false;
	      screen_puts(0, 20, "        \xD9\xA5\x19"
			  "Eject\xD9\xA8\x19Hosts\xD9\xAE\x19New          ");
	      break;
            case 0x9B: // ENTER
	      *selected_host = i;
	      if (hostSlots.host[i][0] != 0x00)
                {
		  // Write hosts
		  fuji_sio_write_host_slots(&hostSlots);
		  
		  // Mount host
		  fuji_sio_mount_host(i, &hostSlots);		  
		  ret = true;
                }
	      else
		ret = false;
	      host_done = true;
	      slot_done = true;
	      break;
            } // switch
	  
	  if (k > 0)
            {
	      bar_clear();
	      bar_show(i + 2);
	      k = 0;
            }
        }
      prev_consol = GTIA_READ.consol;
    }
  
  bar_clear();
  if (slot_done == false)
    bar_show(13);
  
  bar_clear();
  bar_show(i + 13);
  
  k = 0;
  
  // Keyboard input in DRIVES area
  while (slot_done == false)
    {
      // Quick boot
      if (GTIA_READ.consol == 0x03)
        {
	  fuji_sio_mount_all_hosts(&deviceSlots,&hostSlots);
	  fuji_sio_mount_all_devices(&deviceSlots);
	  cold_boot();
        }
      
      if (kbhit())
        {
	  k = cgetc();
	  switch (k)
            {
            case 0x1C: // ATASCII UP
            case '-':
	      if (i > 0)
		i--;
	      break;
            case 0x1D: // ATASCII DOWN
            case '=':
	      if (i < 7)
		i++;
	      break;
            case '_': // SHIFT + UP ARROW
	      color_luminanceIncrease();
	      break;
            case '|': // SHIFT + DOWN ARROW
	      color_luminanceDecrease();
	      break;
            case '\\': // SHIFT + LEFT ARROW
	      color_hueDecrease();
	      break;
            case '^': // SHIFT + RIGHT ARROW
	      color_hueIncrease();
	      break;
            case '!': // Hosts 1-7
            case '"':
            case '#':
            case '$':
            case '%':
            case '&':
            case '\'':
	      i = k - '!';
	      slot_done = true;
	      host_done = false;
	      goto rehosts_jump;
	      break;
            case '@': // Special case for host 8
	      i = 7;
	      slot_done = true;
	      host_done = false;
	      goto rehosts_jump;
	      break;
            case 'C': // Config
            case 'c':
	      host_done = true;
	      slot_done = true;
	      ret = false;
	      info_run();
	      break;
            case 'H': // Hosts
            case 'h':
	      slot_done = true;
	      host_done = false;
	      goto rehosts;
            case 'E': // Eject
            case 'e':
            doeject:
	      fuji_sio_umount_device(i);
	      screen_puts(0, i + 11, " ");
	      screen_puts(3, i + 11, "  Empty                              ");
	      memset(deviceSlots.slot[i].file, 0, sizeof(deviceSlots.slot[i].file));
	      deviceSlots.slot[i].hostSlot = 0xFF;
	      fuji_sio_write_device_slots(&deviceSlots);
	      break;
            case 'N': // New disk
            case 'n':
	      screen_puts(4, i + 11, "                                    ");
	      screen_puts(0, 20, "Enter filename of new ATR image        ");
	      screen_puts(0, 21, "                                       ");
	      memset(tmp_str, 0, sizeof(tmp_str));
	      memset(deviceSlots.slot[i].file, 0, sizeof(deviceSlots.slot[i].file));
	      
	      // #8bitandmore
	      retval = screen_input(4, i + 11, deviceSlots.slot[i].file);
	      if (strcmp(deviceSlots.slot[i].file, "") == 0 || retval == -1)
                {
		  fuji_sio_umount_device(i);
		  screen_puts(0, i + 11, " ");
		  screen_puts(3, i + 11, "  Empty                              ");
		  memset(deviceSlots.slot[i].file, 0, sizeof(deviceSlots.slot[i].file));
		  deviceSlots.slot[i].hostSlot = 0xFF;
		  fuji_sio_write_device_slots(&deviceSlots);
		  screen_puts(0, 20, "                                       ");
		  host_done = true;
		  slot_done = false;
		  screen_puts(0, 20, "        \xD9\xA5\x19"
			      "Eject\xD9\xA8\x19Hosts\xD9\xAE\x19New          ");
		  screen_puts(0, 21, "    \xD9\x91\x8D\x98\x19"
			      "Drives \xD9\xDC\x91\x8D\x98\x19"
			      "Hosts\xD9\xA3\x19"
			      "Config");
		  break;
                }
	      //
	      
	      screen_puts(0, 20, "Which Host Slot (1-8)?                 ");
	      screen_puts(0, 21, "                                       ");
	      memset(tmp_str, 0, sizeof(tmp_str));
	      screen_input(23, 20, tmp_str);
	      deviceSlots.slot[i].hostSlot = atoi(tmp_str);
	      deviceSlots.slot[i].hostSlot -= 1;
	      fuji_sio_mount_host(deviceSlots.slot[i].hostSlot,&hostSlots);
	      screen_puts(0, 20, "Size?\xD9\x91\x19"
			  "90K  \xD9\x92\x19"
			  "130K  \xD9\x93\x19"
			  "180K  \xD9\x94\x19"
			  "360K  ");
	      screen_puts(0, 21, "     \xD9\x95\x19"
			  "720K \xD9\x96\x19"
			  "1440K \xD9\x97\x19"
			  "Custom          ");
	      memset(tmp_str, 0, sizeof(tmp_str));
	      screen_input(32, 21, tmp_str);
	      disk_type = atoi(tmp_str);
	      if (disk_type == 1)
                {
		  ns = 720;
		  ss = 128;
                }
	      else if (disk_type == 2)
                {
		  ns = 1040;
		  ss = 128;
                }
	      else if (disk_type == 3)
                {
		  ns = 720;
		  ss = 256;
                }
	      else if (disk_type == 4)
                {
		  ns = 1440;
		  ss = 256;
                }
	      else if (disk_type == 5)
                {
		  ns = 2880;
		  ss = 256;
                }
	      else if (disk_type == 6)
                {
		  ns = 5760;
		  ss = 256;
                }
	      else if (disk_type == 7)
                {
		  screen_puts(0, 20, "# Sectors?                            ");
		  screen_puts(0, 21, "                                      ");
		  memset(tmp_str, 0, sizeof(tmp_str));
		  screen_input(12, 20, tmp_str);
		  ns = atoi(tmp_str);
		  screen_puts(0, 21, "Sector size (128/256)?                ");
		  memset(tmp_str, 0, sizeof(tmp_str));
		  screen_input(24, 21, tmp_str);
		  ss = atoi(tmp_str);
                }
	      
	      memset(tmp_str, 0, sizeof(tmp_str));
	      screen_puts(0, 20, "Are you sure (Y/N)?                     ");
	      screen_puts(0, 21, "                                        ");
	      screen_input(21, 20, tmp_str);
	      
	      if ((tmp_str[0] == 'Y') || (tmp_str[0] == 'y'))
                {
		  screen_puts(0, 20, "Creating new Disk                       ");
		  screen_puts(0, 21, "                                        ");
		  fuji_sio_new_disk(i, ns, ss, &deviceSlots);
		  
		  if (OS.dcb.dstats != 1)
		    goto doeject;
		  
		  fuji_sio_write_device_slots(&deviceSlots);
		  goto rehosts;
                }
	      else
                    goto doeject;
	      break;
            } // switch
	  
	  if (k > 0)
            {
	      bar_clear();
	      bar_show(i + 13);
	      k = 0;
            }
        }
      prev_consol = GTIA_READ.consol;
    }
  return ret;
}
