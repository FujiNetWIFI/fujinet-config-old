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

typedef enum _substate
  {
   SELECT_SLOT,
   DONE
  } SubState;

void diskulator_copy_setup(Context *context)
{
  screen_dlist_diskulator_copy();
  screen_clear();
  
  memset(context->directory_destination, 0, sizeof(context->directory_destination));

  context->copyFile = true;

  screen_puts(7, 0, "COPY TO:");

  screen_puts(0, 21,
	      CH_KEY_UP "Up"
	      CH_KEY_DOWN "Down"
	      CH_KEY_RETURN "Select");


  diskulator_select_display_directory_path(context);
  diskulator_hosts_display_host_slots(context);  
}

State diskulator_copy(Context *context)
{
  SubState ss = SELECT_SLOT;

  diskulator_copy_setup(context);

  return context->state;
}
