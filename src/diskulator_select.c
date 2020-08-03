/**
 * #FUJINET CONFIG
 * Diskulator Select Disk image screen
 */

#include "diskulator_select.h"

typedef enum _substate
  {
   SELECT_FILE,
   DONE
  } SubState;

/**
 * Diskulator select disk image
 */
State diskulator_select(Context *context)
{
  State new_state = DISKULATOR_HOSTS;
  SubState ss=SELECT_FILE;
  
  
  return new_state;
}
