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
  unsigned char k=0;
  unsigned char i=0;
  
  screen_dlist_diskulator_copy_destination_host_slot();

  strcat(context->full_path,context->directory);
  strcat(context->full_path,context->filename);
  
  screen_puts(0,22,
      CH_KEY_1TO8 "Slot"
      CH_KEY_RETURN "Select"
      CH_KEY_ESC "Abort");

  screen_puts(0,19,context->full_path);

  screen_puts(0,0,"COPY TO HOST SLOT");

  fuji_sio_read_host_slots(&context->hostSlots);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_HOST_SLOTS);

  diskulator_hosts_display_host_slots(2,context);
  bar_show(3);

  while (context->copySubState == SELECT_HOST_SLOT)
    {
      k=input_handle_key();
      input_handle_nav_keys(k,3,8,&i);

      switch(k)
	{
	case 0x1b:
	  context->copySubState = DISABLED;
	  context->state = DISKULATOR_SELECT;
	  break;
	case 0x9b:
	  context->copySubState = SELECT_DESTINATION_FOLDER;
	  context->state = DISKULATOR_SELECT;
	  context->host_slot_dest=i-0x30;
	  break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
          i=k-=0x31;
          bar_show(i+3);
        default:
          break;
	}
    }
}

/**
 * Destination device copy screen
 */
State diskulator_copy(Context *context)
{
  while (context->state == DISKULATOR_COPY)
    {
      if (context->copySubState == SELECT_HOST_SLOT)
	  diskulator_copy_destination_host_slot(context);
      else
	context->state = DISKULATOR_SELECT;
    }
  return context->state;
}
