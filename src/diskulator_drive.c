/**
 * Diskulator - drive selection screen
 */

#include <atari.h>
#include "diskulator_drive.h"
#include "screen.h"
#include "fuji_typedefs.h"
#include "color.h"

extern HostSlots hostSlots;     // diskulator.c
extern DeviceSlots deviceSlots; // diskulator.c

/**
 * Display device slots
 */
void diskulator_drive_display_device_slots(void)
{
  unsigned char i;

  fuji_sio_read_device_slots(&deviceSlots);

  if (OS.dcb.dstats > 128)
    {
      screen_puts(0,21,"COULD NOT READ DEVICE SLOTS!");
      die();
    }
  
  for (i = 0; i < 8; i++)
    {
      unsigned char d[4];
      d[0] = 'D';
      d[1] = '1' + i;
      d[2] = ':';
      d[3] = 0x00;
      screen_puts(0, i + 2, d);
      screen_puts(4, i + 2, deviceSlots.slot[i].file[0] != 0x00 ? deviceSlots.slot[i].file : "Empty");
    }
}

/**
 * Device selection screen setup
 */
void diskulator_drive_screen_setup(void)
{
  screen_clear();
  bar_clear();

  // Patch display list
  POKE(0x60F, 2);
  POKE(0x610, 2);

  screen_puts(0, 0, "MOUNT TO DRIVE SLOT");
  screen_puts(0, 21, "\xD9\x91\x8D\x98\x80\xAF\xB2\x80\xB2\xA5\xB4\xB5\xB2\xAE\x19PICK");
  screen_puts(20, 21, "\xD9\xA5\xB3\xA3\x19"
	      "ABORT  \xD9\xA5\x19"
	      "EJECT ");
}

/**
 * Handle keys for device selection screen
 */
void diskulator_drive_handle_keys(unsigned char k, unsigned char* i)
{
  switch(k)
    {
    case 0x1C:
    case '-':
      diskulator_cursor_up(i);
      break;
    case 0x1D:
    case '=':
      diskulator_cursor_down(i,7);
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
      break;
    }

  if (k>0)
    bar_show(*i+3);
}

/**
 * Select destination drive
 */
void diskulator_drive(void)
{
  unsigned char i=0;
  
  diskulator_drive_screen_setup();
  diskulator_drive_display_device_slots();

  while (true)
    {
      k=diskulator_process_key();
      if (diskulator_drive_handle_keys(k,i)==true)
	break;
    }
  
    bar_show(c + 3);

    while (drive_done == false)
    {
        k = cgetc();
        switch (k)
        {
        case 0x1C: // ATASCII UP
        case '-':
            if (c > 0)
                c--;
            break;
        case 0x1D: // ATASCII DOWN
        case '=':
            if (c < 7)
                c++;
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
        case 'E': // Eject
        case 'e':
            screen_puts(4, c + 2, "Empty                               ");
            memset(deviceSlots.slot[c].file, 0, sizeof(deviceSlots.slot[c].file));
            deviceSlots.slot[c].hostSlot = 0xFF;
            diskulator_write_device_slots();
            break;
        case 0x1B: // ESC
            drive_done = true;
            break;
        case '1': // Drives 1-8
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            c = k - '1';
            bar_clear();
            bar_show(c + 3);
            goto rorw;
            break;
        case 0x9B: // RETURN
        rorw:
            screen_puts(0, 21, "\xD9\xB2\xA5\xB4\xB5\xB2\xAE\x19R/O     \xD9\xB7\x19R/W     \xD9\xA5\xB3\xA3\x19"
                               "ABORT             ");
            //screen_puts(11, 21, "");

            o = 0;

            k = cgetc();

            if ((k == 'R') || (k == 'r'))
                o |= 0x01;
            else if ((k == 'W') || (k == 'w'))
                o |= 0x02;
            else if (k == 0x1B)
                goto drive_slot_abort;

            deviceSlots.slot[c].hostSlot = selected_host;
            deviceSlots.slot[c].mode = o;
            strcpy(deviceSlots.slot[c].file, path);

            diskulator_write_device_slots();
            diskulator_mount_device(c, o);
        drive_slot_abort:
            drive_done = true;
            break;
        }
        if (k > 0)
        {
            bar_clear();
            bar_show(c + 3);
            k = 0;
        }
    }
}
