/**
 * #FUJINET CONFIG
 * Diskulator Destination Device Copy screen
 * essentially, this is the destination host slot screen.
 */

#include "diskulator_copy.h"
#include <atari.h>
#include <string.h>
#include <conio.h>
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
 * Do copy
 */
void diskulator_copy_do(Context *context)
{
  screen_clear();

  // Build copyspec to send to fuji_sio_copy
  strcat(context->full_path,"|"); // add delimiter
  strcat(context->full_path,context->directory);

  screen_puts(0,0,"COPYING, PLEASE WAIT");

  fuji_sio_copy(context->host_slot_source,context->host_slot_dest,context->full_path);

  if (OS.dcb.dstats == 138)
    {                 
      screen_puts(0,2,"#FujiNet is still copying the file.");
      screen_puts(0,3,"Please wait until the activity light");
      screen_puts(0,4,"is done, before pressing a key");
      screen_puts(0,5,"to continue.");
      while (!kbhit()) {}
    }
  else if (OS.dcb.dstats == 144)
    {
      screen_puts(0,2,"There was a problem copying the file.");
      screen_puts(0,3,"You may need to ensure permissions");
      screen_puts(0,4,"Are sufficient, before copying the file again.");
      screen_puts(0,5,"Press any key to continue.");
      while (!kbhit()) {}
    }
  
  // Clear the path for next time.
  memset(context->full_path,0,sizeof(context->full_path));
  context->state = DISKULATOR_SELECT;
  context->copySubState = DISABLED;
}

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
          fuji_sio_mount_host(i,&context->hostSlots);

          if (fuji_sio_error())
            {
              error(ERROR_MOUNTING_HOST_SLOT);
              wait_a_moment();
              context->state=CONNECT_WIFI;
	      return;
	    }
	  
	  context->copySubState = SELECT_DESTINATION_FOLDER;
	  context->state = DISKULATOR_SELECT;
	  context->host_slot_source=context->host_slot+1;
	  context->host_slot=i;
 	  context->host_slot_dest=i+1;
	  context->dir_page=0;
	  memset(context->directory,0,sizeof(context->directory));
	  strcpy(context->directory, "/");
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
      else if (context->copySubState == DO_COPY)
	diskulator_copy_do(context);
      else
	context->state = DISKULATOR_SELECT;
    }
  return context->state;
}
