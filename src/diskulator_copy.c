/**
 * #FUJINET CONFIG
 * Diskulator Copy To screen
 */

#include <atari.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "screen.h"
#include "fuji_sio.h"
#include "die.h"
#include "bar.h"
#include "input.h"
#include "error.h"
#include "diskulator_select.h"
#include "diskulator_hosts.h"

void diskulator_copy_slot(Context *context)
{
  unsigned char k=0;
  unsigned char i=0;

  while (context->copyFile == SELECT_DESTINATION_SLOT)
    {
      k=input_handle_key();
      input_handle_nav_keys(k,7,8,&i);

      switch(k)
	{
	case 0x1b:
	  context->copyFile=DISABLED;
	  context->state=DISKULATOR_SELECT;
	  break;
	case 0x9B:
	  context->state=DISKULATOR_SELECT;
	  context->copyFile=SELECT_DESTINATION_DIR;
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
	  bar_show(i+7);
	  break;
	default:
	  break;
	}
    }
}

void diskulator_copy_do(Context *context)
{
  
}

void diskulator_copy_setup(Context *context)
{
  screen_dlist_diskulator_copy();
  screen_clear();
  
  memset(context->directory_destination, 0, sizeof(context->directory_destination));

  context->copyFile = true;

  screen_puts(6, 0, "COPY FROM:");

  screen_puts(7 ,4, "COPY TO:");
  
  screen_puts(0, 21,
	      CH_KEY_UP "Up"
	      CH_KEY_DOWN "Down"
	      CH_KEY_RETURN "Select"
	      CH_KEY_ESC "Abort");


  diskulator_select_display_directory_path(context);
  diskulator_hosts_display_host_slots(4,context);  
}

State diskulator_copy(Context *context)
{
  diskulator_copy_setup(context);
  bar_show(7);

  while (context->copyFile != DISABLED)
    {
      switch(context->copyFile)
	{
	case SELECT_DESTINATION_SLOT:
	  diskulator_copy_slot (context);
	  break;
	case DO_COPY:
	  diskulator_copy_do(context);
	  break;
	}
    }
  
  return context->state;
}
