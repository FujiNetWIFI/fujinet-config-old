/**
 * #FUJINET CONFIG
 * Diskulator Destination Device Copy screen
 * essentially, this is the destination host slot screen.
 */

#include "diskulator_copy.h"
#include <atari.h>
#include <string.h>
#include "diskulator_hosts.h"
#include "diskulator_slot.h"
#include "fuji_sio.h"
#include "fuji_typedefs.h"
#include "screen.h"
#include "error.h"
#include "bar.h"
#include "input.h"
#include "die.h"

/**
 * Destination device copy setup
 */
void diskulator_copy_destination_host_slot(Context *context)
{
  screen_dlist_diskulator_copy_destination_host_slot();

  screen_puts(0,22,
      CH_KEY_1TO8 "Slot"
      CH_KEY_RETURN "Select"
      CH_KEY_LABEL_L CH_INV_E CH_KEY_LABEL_R "ject"
      CH_KEY_ESC "Abort");

  screen_puts(0,19,context->filename);

  screen_puts(0,0,"MOUNT TO DRIVE SLOT");

  fuji_sio_read_device_slots(&context->deviceSlots);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_DEVICE_SLOTS);

  diskulator_hosts_display_device_slots(2,context);
  bar_show(3);
}

/**
 * Destination device copy screen
 */
State diskulator_copy(Context *context)
{
  while (context->state = DISKULATOR_COPY)
    {
      if (context->copySubState == SELECT_HOST_SLOT)
	  diskulator_copy_destination_host_slot(context);
      else
	context->state = DISKULATOR_SELECT;
    }
  return context->state;
}
