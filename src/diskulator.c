/**
 * Fujinet Configurator
 *
 * Diskulator
 */

#include <atari.h>
#include <string.h>
#include <peekpoke.h>
#include <conio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "diskulator.h"
#include "diskulator_host.h"
#include "screen.h"
#include "sio.h"
#include "bar.h"
#include "die.h"
#include "info.h"
#include "color.h"

unsigned char prefix[256] = "/";
unsigned char path[256];
unsigned char current_entry[FILE_MAXLEN];
unsigned char c;
unsigned char o;
unsigned char files[FILES_MAXCOUNT][FILE_MAXLEN];
unsigned char diskulator_done = false;
unsigned char selected_host;
unsigned char filter[32];
unsigned char prev_consol;
bool host_done = false;
bool slot_done = true;
bool selector_done = false;
bool drive_done = false;
unsigned char k;
unsigned char zx[2];

int retval = 0;

extern unsigned char *video_ptr;
extern unsigned char *dlist_ptr;
extern unsigned short screen_memory;
extern unsigned char *font_ptr;

/**
 * Do coldstart
 */
void diskulator_boot(void)
{
    asm("jmp $E477");
}

/**
 * Mount all Hosts
 */
void diskulator_mount_all_hosts(void)
{
    unsigned char e;

    bar_clear();
    screen_clear();

    screen_puts(0, 0, "MOUNTING ALL HOSTS");

    for (e = 0; e < 8; e++)
    {
        if (deviceSlots.slot[e].hostSlot != 0xFF)
        {
            diskulator_mount_host(deviceSlots.slot[e].hostSlot);
        }

        if (OS.dcb.dstats != 0x01)
        {
            screen_puts(0, 21, "MOUNT ERROR!");
            die();
        }
    }
}

/**
 * Mount all devices
 */
void diskulator_mount_all_devices(void)
{
    unsigned char e;

    bar_clear();
    screen_clear();

    screen_puts(0, 0, "MOUNTING ALL DEVICES");

    for (e = 0; e < 8; e++)
    {
        if (deviceSlots.slot[e].hostSlot != 0xFF)
            diskulator_mount_device(e, deviceSlots.slot[e].mode);

        if (OS.dcb.dstats != 0x01)
        {
            screen_puts(0, 21, "MOUNT ERROR!");
            die();
        }
    }
}

/**
 * Select an image
 */
bool diskulator_select(void)
{
    unsigned char num_entries;
    unsigned char e;
    bool ret = false;

    POKE(0x60F, 2);
    POKE(0x610, 2);

subdir:
    selector_done = false;
    screen_clear();
    bar_clear();

    screen_puts(0, 0, "    DISK IMAGES    ");

    screen_puts(0, 21, "\xD9\xB2\xA5\xB4\xB5\xB2\xAE\x19"
                       "PICK \xD9\xA5\xB3\xA3\x19"
                       "ABORT");
    screen_puts(24, 21, "                  ");

    screen_puts(0, 1, prefix);

    current_entry[0] = '\0';

    diskulator_open_directory(selected_host, prefix);

    num_entries = 0;
    while ((current_entry[0] != 0x7F) && (num_entries < FILES_MAXCOUNT))
    {
        diskulator_read_directory(selected_host, current_entry, FILE_MAXLEN);
        if (current_entry[0] == '.')
            continue;
        else
        {
            if (current_entry[0] == 0x7F)
                break;
            else
            {
                strncpy(files[num_entries], current_entry, FILE_MAXLEN);

                if (current_entry[strlen(current_entry) - 1] == '/')
                    screen_puts(0, num_entries + 2, "\x04");

                screen_puts(2, num_entries + 2, current_entry);
                num_entries++;
            }
        }
    }

    diskulator_close_directory(selected_host);

    e = 0;
    bar_clear();
    bar_show(e + 3);

    selector_done = false;

    while (1)
    {
        if (kbhit())
	  {
	    rtclr();
            k = cgetc();
	  }
	else if (OS.rtclok[2]>100)
	  {
	    bar_set_color(0x44);
	  }
	
        switch (k)
        {
        case 0x1C: // Up
        case '-':
            if (e > 0)
                e--;
            break;
        case 0x1D: // Down
        case '=':
            if (e < num_entries - 1)
                e++;
            break;
        case 95: // SHIFT + UP ARROW
            color_luminanceIncrease();
            break;
        case 124: // SHIFT + DOWN ARROW
            color_luminanceDecrease();
            break;
        case 92: // CTRL + LEFT ARROW
            color_hueDecrease();
            break;
        case 94: // CTRL + RIGHT ARROW
            color_hueIncrease();
            break;
        case 0x1B: // ESC
            selector_done = true;
            memset(path, 0, sizeof(path));
            bar_set_color(0x97);
            ret = false;
            goto image_done;
            break;
        case 0x9B: // Enter
            bar_set_color(0x97);

            if (files[e][strlen(files[e]) - 1] == '/') // subdir
            {
                selector_done = false;
                strcat(prefix, files[e]);
                k = 0;
                goto subdir;
            }

            memset(path, 0, sizeof(path));
            strcat(path, prefix);
            strcat(path, files[e]);
            ret = true;
            goto image_done;
            break;
        }
        if (k > 0)
        {
            bar_clear();
            bar_show(e + 3);
            k = 0;
        }
    }
image_done:
    return ret;
}

/**
 * Select destination drive
 */
void diskulator_drive(void)
{
    unsigned char c, k;

    drive_done = false;

    POKE(0x60F, 2);
    POKE(0x610, 2);
    //POKE(0x61B, 6);
    //POKE(0x61C, 6);

    screen_clear();
    bar_clear();

    screen_puts(0, 0, "MOUNT TO DRIVE SLOT");
    screen_puts(0, 21, "\xD9\x91\x8D\x98\x80\xAF\xB2\x80\xB2\xA5\xB4\xB5\xB2\xAE\x19PICK");
    screen_puts(20, 21, "\xD9\xA5\xB3\xA3\x19"
                        "ABORT  \xD9\xA5\x19"
                        "EJECT ");
    diskulator_read_device_slots();

    // Display drive slots
    for (c = 0; c < 8; c++)
    {
        unsigned char d[4];
        d[0] = 'D';
        d[1] = 0x31 + c;
        d[2] = ':';
        d[3] = 0x00;
        screen_puts(0, c + 2, d);
        screen_puts(4, c + 2, deviceSlots.slot[c].file[0] != 0x00 ? deviceSlots.slot[c].file : "Empty");
    }

    c = 0;

    bar_clear();
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

/**
 * Run the Diskulator
 */
void diskulator_run(void)
{
    if (GTIA_READ.consol == 0x04) // Option
    {
        diskulator_read_host_slots();
        diskulator_read_device_slots();
        diskulator_mount_all_hosts();
        diskulator_mount_all_devices();
        diskulator_boot();
    }

    while (diskulator_done == false)
    {
        if (diskulator_host() == true)
        {
            if (diskulator_select() == true)
            {
                diskulator_drive();
            }
        }
    }
    diskulator_boot();
}
