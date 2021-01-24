/**
 * #FUJINET CONFIG
 * Destination device slot screen.
 */

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
  screen_dlist_diskulator_slot();

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
        case 'E':
        case 'e':
          diskulator_hosts_eject_device_slot(i,4,context);
          break;
        case 0x1B:
          *ss=DONE;
          context->state=DISKULATOR_SELECT;
          break;
        case 0x9B:
          context->device_slot=i;
          *ss=(context->newDisk==true ? CREATE_DISK : SELECT_MODE);
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
 * Select mode for device slot
 */
void diskulator_slot_select_mode(Context *context, SubState *ss)
{
  unsigned char k=0;
  screen_clear_line(21);

  screen_puts(0,22,
    CH_KEY_RETURN "Read Only"
    CH_KEY_LABEL_L CH_INV_W CH_KEY_LABEL_R "Read/Write"
    CH_KEY_ESC "Abort");

  while (k==0)
    k=input_handle_key();

  switch (k)
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
    {
      error(ERROR_CREATING_NEW_DISK);
      wait_a_moment();
      context->state=DISKULATOR_SELECT;
      return;
    }

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
    {
      error(ERROR_WRITING_DEVICE_SLOTS);
      wait_a_moment();
      context->state=DISKULATOR_HOSTS;
      return;
    }

  // Write full filename
  fuji_sio_set_filename_for_device_slot(context->device_slot,context->full_path);
  if (fuji_sio_error())
    {
      error(ERROR_SETTING_FULL_PATH);
      wait_a_moment();
      context->state=DISKULATOR_HOSTS;
      return;
    }

  // And go back to the hosts/slots screen
  *ss=DONE;
  context->state=DISKULATOR_SELECT;
}

/**
 * Connect wifi State
 */
State diskulator_slot(Context *context)
{
  SubState ss=SELECT_SLOT;

  diskulator_slot_setup(context);

  if (strstr(context->filename,".cas") != NULL)
    {
      ss = COMMIT_SLOT;
      context->device_slot = 7;
      bar_show(10);
    }
  
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
