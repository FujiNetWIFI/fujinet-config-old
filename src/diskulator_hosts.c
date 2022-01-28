/**
 * #FUJINET CONFIG
 * Diskulator Hosts/Devices
 */

#include <stdlib.h>
#include <string.h>
#include "diskulator_hosts.h"
#include "screen.h"
#include "fuji_sio.h"
#include "fuji_typedefs.h"
#include "error.h"
#include "die.h"
#include "input.h"
#include "bar.h"

char text_empty[]="Empty";
char fn[256];

typedef enum _substate
  {
   HOSTS,
   DEVICES,
   DONE
  } SubState;

#define HOST_SLOTS_Y 1

/**
 * Display Hosts Slots
 */
void diskulator_hosts_display_host_slots(unsigned char y, Context *context)
{
  unsigned char i;

  // Display host slots
  for (i = 0; i < 8; i++)
    {
      set_cursor(2,i+y);
      put_char(i+'1');
      cursor_ptr+=2;

      screen_append((context->hostSlots.host[i][0] != 0x00)?
        (char *)context->hostSlots.host[i]: text_empty);
    }
}

/**
 * Display device slots
 */
void diskulator_hosts_display_device_slots(unsigned char y, Context *context)
{
  unsigned char i;
  unsigned char d[6];

  // Get full filename for device slot 8
  if (context->deviceSlots.slot[7].file[0]!=0x00)
    fuji_sio_get_filename_for_device_slot(7,fn);

  // Display device slots
  for (i = 0; i < 8; i++)
    {
      d[1] = 0x20;
      d[2] = (i==7 && strstr(fn,".cas") != NULL) ? 'C' : (0x31 + i);
      d[4] = 0x20;
      d[5] = 0x00;

      if (context->deviceSlots.slot[i].file[0] != 0x00)
        {
          d[0] = context->deviceSlots.slot[i].hostSlot + 0x31;
          d[3] = (context->deviceSlots.slot[i].mode == 0x02 ? 'W' : 'R');
        }
      else
        {
          d[0] = 0x20;
          d[3] = 0x20;
        }

      screen_puts(0, i + y, d);

      screen_append(context->deviceSlots.slot[i].file[0] != 0x00?
        (char *)context->deviceSlots.slot[i].file: text_empty);
    }    
}

/**
 * Keys for Hosts mode
 */
void diskulator_hosts_keys_hosts(void)
{
  screen_clear_line(20);
  screen_clear_line(21);
  screen_puts(0,20,
      CH_KEY_1TO8 "Slot"
      CH_KEY_LABEL_L CH_INV_E CH_KEY_LABEL_R "dit Slot"
      CH_KEY_RETURN "Select Files");
  screen_puts(2,21,
      CH_KEY_LABEL_L CH_INV_C CH_KEY_LABEL_R "onfig"
      CH_KEY_TAB "Drive Slots"
      CH_KEY_OPTION "Boot");
}

/**
 * Keys for Devices mode
 */
void diskulator_hosts_keys_devices(void)
{
  screen_clear_line(20);
  screen_clear_line(21);

  screen_puts(3,20,
    CH_KEY_1TO8 "Slot"
    CH_KEY_LABEL_L CH_INV_E CH_KEY_LABEL_R "ject"
    CH_KEY_LABEL_L CH_INV_C CH_INV_L CH_INV_E CH_INV_A CH_INV_R CH_KEY_LABEL_R "All Slots");
  screen_puts(3,21,
    CH_KEY_TAB "Hosts"
    CH_KEY_LABEL_L CH_INV_R CH_KEY_LABEL_R "ead "
    CH_KEY_LABEL_L CH_INV_W CH_KEY_LABEL_R "rite"
    CH_KEY_LABEL_L CH_INV_C CH_KEY_LABEL_R "onfig");
}

/**
 * Diskulator hosts setup
 */
void diskulator_hosts_setup(Context *context)
{
  unsigned char retry=5;

  screen_dlist_diskulator_hosts();

  screen_puts(3, 0, "TNFS HOST LIST");
  screen_puts(24, 9, "DRIVE SLOTS");

  while (retry>0)
    {
      fuji_sio_read_host_slots(&context->hostSlots);

      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error())
    error_fatal(ERROR_READING_HOST_SLOTS);

  retry=5;

  diskulator_hosts_display_host_slots(HOST_SLOTS_Y,context);

  while (retry>0)
    {
      fuji_sio_read_device_slots(&context->deviceSlots);

      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error())
    error_fatal(ERROR_READING_DEVICE_SLOTS);

  diskulator_hosts_display_device_slots(11,context);

  diskulator_hosts_keys_hosts();

  bar_show(2);
}

/**
 * Handle jump keys (1-8, shift 1-8)
 */
void diskulator_hosts_handle_jump_keys(unsigned char k,unsigned char *i, SubState *ss)
{
  if (k>='1' && k<='8')
    {
      *i=k-'1';
      bar_show((*i)+(*ss==DEVICES ? ORIGIN_DEVICE_SLOTS : ORIGIN_HOST_SLOTS));
    }
}

/**
 * Handle nav keys depending on sub-state (HOSTS or DEVICES)
 */
void diskulator_hosts_handle_nav_keys(unsigned char k, unsigned char *i, SubState *new_substate)
{
  unsigned char o;

  o=*new_substate==DEVICES?
    ORIGIN_DEVICE_SLOTS:
    ORIGIN_HOST_SLOTS;

  input_handle_nav_keys(k,o,8,i);
}

/**
 * Edit a host slot
 */
void diskulator_hosts_edit_host_slot(unsigned char i, Context *context)
{
  if (context->hostSlots.host[i][0] == 0x00)
    {
      char tmp[2]={0,0};
      screen_clear_line(i+1);
      tmp[0]=i+0x31;
      screen_puts(2,i+1,tmp);
    }
  screen_input(5, i+1, context->hostSlots.host[i]);
  if (context->hostSlots.host[i][0] == 0x00)
    screen_puts(5, i+1, text_empty);
  fuji_sio_write_host_slots(&context->hostSlots);
}

/**
 * Eject image from device slot
 */
void diskulator_hosts_eject_device_slot(unsigned char i, unsigned char pos, Context *context)
{
  char tmp[2]={0,0};

  tmp[0]=i+'1'; // string denoting now ejected device slot.

  fuji_sio_umount_device(i);
  memset(context->deviceSlots.slot[i].file,0,sizeof(context->deviceSlots.slot[i].file));
  context->deviceSlots.slot[i].hostSlot=0xFF;
  fuji_sio_write_device_slots(&context->deviceSlots);

  screen_clear_line((i+pos-2));
  screen_puts(2,(i+pos-2),tmp);
  screen_puts(5,(i+pos-2),text_empty);
}

/**
 * Set device slot to read or write
 */
void diskulator_hosts_set_device_slot_mode(unsigned char i, unsigned char mode, Context* context)
{
  unsigned char tmp_hostSlot;
  unsigned char tmp_file[FILE_MAXLEN];

  // temporarily stash current values.
  tmp_hostSlot=context->deviceSlots.slot[i].hostSlot;
  memcpy(tmp_file,context->deviceSlots.slot[i].file,FILE_MAXLEN);
  fuji_sio_get_filename_for_device_slot(i,fn);

  // Unmount slot
  fuji_sio_umount_device(i);

  // Slot is now wiped, need to re-populate from stash.
  context->deviceSlots.slot[i].hostSlot=tmp_hostSlot;
  context->deviceSlots.slot[i].mode=mode;
  memcpy(context->deviceSlots.slot[i].file,tmp_file,FILE_MAXLEN);
  fuji_sio_set_filename_for_device_slot(i,fn);

  fuji_sio_write_device_slots(&context->deviceSlots);
  fuji_sio_mount_device(i,mode);

  // If we couldn't mount read/write, then re-mount again as read-only.
  if (fuji_sio_error())
    {
      fuji_sio_umount_device(i);

      // Slot is now wiped, need to re-populate from stash.
      context->deviceSlots.slot[i].hostSlot=tmp_hostSlot;
      context->deviceSlots.slot[i].mode=mode;
      memcpy(context->deviceSlots.slot[i].file,tmp_file,FILE_MAXLEN);
      fuji_sio_set_filename_for_device_slot(i,fn);

      // Try again.
      fuji_sio_write_device_slots(&context->deviceSlots);
      fuji_sio_mount_device(i,mode);
    }

  // And update device slot display.
  diskulator_hosts_display_device_slots(11,context);
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

      k=input_handle_key_ucase();
      diskulator_hosts_handle_jump_keys(k,&i,new_substate);
      diskulator_hosts_handle_nav_keys(k,&i,new_substate);

      switch(k)
        {
        case '!':
          *new_substate=DONE;
          context->state = MOUNT_AND_BOOT;
          break;
        case 0x7F:
          i=0;
          *new_substate = DEVICES;
          diskulator_hosts_keys_devices();
          bar_show(i+ORIGIN_DEVICE_SLOTS);
          break;
        case 'B':
          *new_substate=DONE;
          context->state = MOUNT_AND_BOOT;
          break;
        case 'C':
          context->state=DISKULATOR_INFO;
          *new_substate=DONE;
          break;
        case 'E':
          diskulator_hosts_edit_host_slot(i,context);
          break;
	case 0x7D: // SHIFT-CLEAR
	  for (i=0;i<8;i++)
	    diskulator_hosts_eject_device_slot(i,ORIGIN_DEVICE_SLOTS,context);
	  break;
        case KCODE_RETURN: // RETURN
          if (context->hostSlots.host[i][0]==0x00 || (context->net_connected == false && strcmp(context->hostSlots.host[i],"SD") != 0)) // empty host slot?
            break; // do nothing
	  
          context->state=DISKULATOR_SELECT;
          context->host_slot=i;
          strcpy(context->host, context->hostSlots.host[context->host_slot]);
          fuji_sio_mount_host(context->host_slot,&context->hostSlots);

          if (fuji_sio_error())
            {
              error(ERROR_MOUNTING_HOST_SLOT);
              wait_a_moment();
              context->state=CONNECT_WIFI;
              *new_substate=DONE;
              return;
            }
          else
            *new_substate=DONE;
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
      k=input_handle_key_ucase();
      diskulator_hosts_handle_jump_keys(k,&i,new_substate);
      diskulator_hosts_handle_nav_keys(k,&i,new_substate);

      switch(k)
        {
        case '!':
          *new_substate=DONE;
          context->state = MOUNT_AND_BOOT;
          break;
        case 'C':
          context->state=DISKULATOR_INFO;
          *new_substate=DONE;
          break;
        case 'E':
          diskulator_hosts_eject_device_slot(i,ORIGIN_DEVICE_SLOTS,context);
          break;
	case 0x7D: // SHIFT-CLEAR
	  for (i=0;i<8;i++)
	    diskulator_hosts_eject_device_slot(i,ORIGIN_DEVICE_SLOTS,context);
	  break;
        case 0x7F:
          i=0;
          *new_substate = HOSTS;
          diskulator_hosts_keys_hosts();
          bar_show(i+ORIGIN_HOST_SLOTS);
          break;
        case 'R':
          diskulator_hosts_set_device_slot_mode(i,MODE_READ,context);
          break;
        case 'W':
          diskulator_hosts_set_device_slot_mode(i,MODE_WRITE,context);
          break;
        }
    }
}

/**
 * Clear file context
 */
void diskulator_hosts_clear_file_context(Context *context)
{
  memset(context->directory,0,sizeof(context->directory));
  memset(context->filter,0,sizeof(context->filter));
  context->dir_page=0;
}

/**
 * Connect wifi State
 */
State diskulator_hosts(Context *context)
{
  SubState ss=HOSTS;

  diskulator_hosts_setup(context);
  diskulator_hosts_clear_file_context(context);

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
