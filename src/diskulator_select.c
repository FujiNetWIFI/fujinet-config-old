/**
 * #FUJINET CONFIG
 * Diskulator Select Disk image screen
 */

#include "diskulator_select.h"
#include "screen.h"

typedef enum _substate
  {
   SELECT_FILE,
   DONE
  } SubState;

/**
 * Setup Diskulator Disk Images screen
 */
void diskulator_select_setup(Context *context)
{
  screen_dlist_diskulator_select();
  screen_puts(4, 0, "DISK IMAGES");
  
  screen_puts(0, 21, "\xD9\xB2\xA5\xB4\xB5\xB2\xAE\x19"
	      "PICK \xD9\xA5\xB3\xA3\x19"
	      "ABORT");
  
  screen_puts(0, 1, context->directory); 
}

/**
 * Diskulator select disk image
 */
State diskulator_select(Context *context)
{
  State new_state = DISKULATOR_HOSTS;
  SubState ss=SELECT_FILE;

  diskulator_select_setup(context);
  
  return new_state;
}
