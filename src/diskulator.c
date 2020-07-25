/**
 * Fujinet Configurator
 *
 * Diskulator
 */

#include <stdbool.h>
#include <conio.h>
#include "diskulator.h"
#include "diskulator_host.h"
#include "diskulator_select.h"
#include "diskulator_drive.h"

HostSlots hostSlots;
DeviceSlots deviceSlots;

/**
 * Cursor up for either hosts or devices
 */
void diskulator_cursor_up(unsigned char* i)
{
  if (*i > 0)
    *i--;
}

/**
 * Cursor down for either hosts or devices
 */
void diskulator_cursor_down(unsigned char* i, unsigned char max)
{
  if (*i < max)
    *i++;
}

/**
 * Process key for hosts or devices
 */
char diskulator_process_key(void)
{
  if (kbhit())
    return cgetc();
  else
    return 0;
}

/**
 * Run the Diskulator
 */
void diskulator_run(void)
{
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
