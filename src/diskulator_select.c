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

typedef unsigned short Page;
typedef unsigned char PageOffset;

/**
 * Convert directory position to page and offset
 */
void diskulator_select_pos_to_page(DirectoryPosition pos, Page* page, PageOffset* offset)
{
  *page=pos>>4;
  *offset=pos&0x0F;
}

/**
 * Convert page/offset to directory position
 */
void diskulator_select_page_to_pos(Page page, PageOffset offset, DirectoryPosition* pos)
{
  *pos=(page<<4)+offset;
}

/**
 * Select file
 */
void diskulator_select_select_file(Context* context, SubState* ss)
{
  
}
  
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

  while (ss != DONE)
    {
      switch(ss)
	{
	case SELECT_FILE:
	  diskulator_select_select_file(context,&ss);
	  break;
	}
    }
  
  return new_state;
}
