/**
 * #FUJINET CONFIG
 * Destination device slot screen.
 */

#include <atari.h>
#include <conio.h>
#include <string.h>
#include "diskulator_hosts.h"
#include "diskulator_slot.h"
#include "fuji_sio.h"
#include "fuji_typedefs.h"
#include "screen.h"
#include "error.h"
#include "bar.h"
#include "input.h"

typedef enum _substate
  {
   SELECT_SLOT,
   SELECT_MODE,
   COMMIT_SLOT,
   CREATE_DISK,
   DONE
  } SubState;

/**
 * Setup for Slot screen
 */
void diskulator_slot_setup(Context* context)
{  
  screen_puts(0,0,"MOUNT TO DRIVE SLOT");
  screen_puts(0, 21, "\xD9\x91\x8D\x98\x80\xAF\xB2\x80\xB2\xA5\xB4\xB5\xB2\xAE\x19PICK");
  screen_puts(20, 21, "\xD9\xA5\xB3\xA3\x19"
	      "ABORT  \xD9\xA5\x19"
	      "EJECT ");
  fuji_sio_read_device_slots(&context->deviceSlots);

  diskulator_hosts_display_device_slots(2,&context->deviceSlots);
  bar_show(3);
}

/**
 * Slot select
 */
void diskulator_slot_select(Context *context, SubState *ss)
{
  unsigned char k=0;
  unsigned char i=0;

  while (*ss==SELECT_SLOT)
    {
      k=input_handle_key();
      input_handle_nav_keys(k,3,8,&i);

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
	  i=k-=0x31;
	  bar_show(i+3);
	case 0x9B:
	  context->device_slot=i;
	  *ss=(context->newDisk==true ? CREATE_DISK : SELECT_MODE);
	  break;
	default:
	  break;
	}
    }
}

/**
 * Select mode for device slot
 */
void diskulator_slot_select_mode(Context *context, SubState *ss)
{
  screen_clear_line(21);
  screen_puts(0, 21, "\xD9\xB2\xA5\xB4\xB5\xB2\xAE\x19R/O     \xD9\xB7\x19R/W     \xD9\xA5\xB3\xA3\x19"
	      "ABORT");

  switch (cgetc())
    {
    case 0x1B:
      *ss=DONE;
      break;
    case 'W':
    case 'w':
      context->mode = 2;
      break;
    default:
      context->mode = 1;
      break;
    }

  *ss=COMMIT_SLOT;
}

/**
 * Create new disk
 */
void diskulator_slot_create_disk(Context *context, SubState *ss)
{
  screen_clear_line(20);
  screen_clear_line(21);

  screen_puts(0,21,"CREATING NEW DISK");

  context->deviceSlots.slot[context->device_slot].hostSlot = context->host_slot;
  strcpy(context->deviceSlots.slot[context->device_slot].file, context->directory);
  strcat(context->deviceSlots.slot[context->device_slot].file, context->filename);
  fuji_sio_new_disk(context->device_slot,context->newDisk_ns,context->newDisk_sz,&context->deviceSlots);
  if (fuji_sio_error())
    error_fatal(ERROR_CREATING_NEW_DISK);

  context->mode = 0x02; // R/W

  *ss=COMMIT_SLOT;
}

/**
 * Slot commit
 */
void diskulator_slot_commit(Context *context, SubState *ss)
{
  // Construct full path to write to device slot
  memset(context->full_path,0,sizeof(context->full_path));
  strcat(context->full_path,context->directory);
  strcat(context->full_path,context->filename);

  // Set host slot and mode.
  context->deviceSlots.slot[context->device_slot].hostSlot=context->host_slot;
  context->deviceSlots.slot[context->device_slot].mode=context->mode;

  // Write the device slots.
  fuji_sio_write_device_slots(&context->deviceSlots);
  if (fuji_sio_error())
    error_fatal(ERROR_WRITING_DEVICE_SLOTS);

  // Write full filename
  fuji_sio_set_filename_for_device_slot(context->device_slot,context->full_path);
  if (fuji_sio_error())
    error_fatal(ERROR_SETTING_FULL_PATH);

  // And go back to the hosts/slots screen
  *ss=DONE;
  context->state=DISKULATOR_HOSTS;
}

/**
 * Connect wifi State
 */
State diskulator_slot(Context *context)
{
  SubState ss=SELECT_SLOT;

  diskulator_slot_setup(context);

  while (ss != DONE)
    {
      switch(ss)
	{
	case SELECT_SLOT:
	  diskulator_slot_select(context,&ss);
	  break;
	case SELECT_MODE:
	  diskulator_slot_select_mode(context,&ss);
	  break;
	case COMMIT_SLOT:
	  diskulator_slot_commit(context,&ss);
	  break;
	case CREATE_DISK:
	  diskulator_slot_create_disk(context,&ss);
	  break;
	case DONE:
	  context->state = DISKULATOR_HOSTS;
	  break;
	}
    }
  
  return context->state;
}
