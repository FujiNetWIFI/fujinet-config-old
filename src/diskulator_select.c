/**
 * #FUJINET CONFIG
 * Diskulator Select Disk image screen
 */

#include <atari.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "diskulator_select.h"
#include "screen.h"
#include "fuji_sio.h"
#include "die.h"
#include "bar.h"
#include "input.h"

typedef enum _substate
  {
   SELECT_FILE,
   PREV_PAGE,
   NEXT_PAGE,
   DONE
  } SubState;

typedef unsigned short Page;
typedef unsigned char PageOffset;

#define DIRECTORY_LIST_Y_OFFSET 2
#define DIRECTORY_LIST_SCREEN_WIDTH 36
#define DIRECTORY_LIST_ENTRIES_PER_PAGE 16

/**
 * Display directory entry
 */
bool diskulator_select_display_directory_entry(unsigned char i, char* entry)
{
  if (entry[0]==0x7F)
    return false; // End of directory

  // Display folder icon if directory.
  if (entry[strlen(entry)-1]=='/')
    screen_puts(0,DIRECTORY_LIST_Y_OFFSET+i,"\x04");

  // Display filename
  screen_clear_line(DIRECTORY_LIST_Y_OFFSET+i);
  screen_puts(2,DIRECTORY_LIST_Y_OFFSET+i,entry);
  return true;
}

/**
 * Clear directory page area
 */
void diskulator_select_display_clear_page(void)
{
  unsigned char i;

  for (i=0;i<DIRECTORY_LIST_ENTRIES_PER_PAGE+2;i++)
    screen_clear_line(DIRECTORY_LIST_Y_OFFSET+i);
}

/**
 * Display directory page
 */
unsigned char diskulator_select_display_directory_page(Context* context)
{
  char displayed_entry[DIRECTORY_LIST_SCREEN_WIDTH];
  unsigned char i=0;

  diskulator_select_display_clear_page();

  fuji_sio_open_directory(context->host_slot,context->directory);
  context->dir_eof=false;
  fuji_sio_set_directory_position(context->dir_pos);
  
  if (context->dir_pos > 0)
    {
      screen_puts(0,i+DIRECTORY_LIST_Y_OFFSET,"\x1C");
      screen_puts(2,i+DIRECTORY_LIST_Y_OFFSET,"<PREV PAGE>");
      i++;
    }
  
  for (i=i;i<DIRECTORY_LIST_ENTRIES_PER_PAGE;i++)
    {
      fuji_sio_read_directory(displayed_entry,DIRECTORY_LIST_SCREEN_WIDTH);
      if (!diskulator_select_display_directory_entry(i,displayed_entry))
	{
	  context->dir_eof=true;
	  break;
	}
    }

  // This does not display if EOF happens exactly on the last entry on screen.
  if (i==DIRECTORY_LIST_ENTRIES_PER_PAGE)
    {
      fuji_sio_read_directory(displayed_entry,DIRECTORY_LIST_SCREEN_WIDTH);
      if (displayed_entry[0]!=0x7F)
	{
	  screen_puts(0,i+DIRECTORY_LIST_Y_OFFSET,"\x1D");
	  screen_puts(2,i+DIRECTORY_LIST_Y_OFFSET,"<NEXT PAGE>");
	  i++;
	}
    }
    
  fuji_sio_close_directory(context->host_slot);
  return i; // Number of entries displayed
}

/**
 * Handle RETURN key - Select item.
 */
void diskulator_select_handle_return(unsigned char i, Context* context, SubState *ss)
{
  if ((i==0) && (context->dir_pos > 0))
    *ss = PREV_PAGE;
  else if (i==16)
    *ss = NEXT_PAGE;
}

/**
 * Select file
 */
void diskulator_select_select_file(Context* context, SubState* ss)
{
  unsigned char mi=diskulator_select_display_directory_page(context);
  unsigned char k=0;
  unsigned char i=0;

  bar_show(DIRECTORY_LIST_Y_OFFSET+1);
  
  while (*ss==SELECT_FILE)
    {
      k=input_handle_key();
      input_handle_nav_keys(k,DIRECTORY_LIST_Y_OFFSET+1,mi,&i);

      switch(k)
	{
	case 0x9B:
	  diskulator_select_handle_return(i,context,ss);
	  break;
	case '<':
	  if (context->dir_pos>0)
	    *ss=PREV_PAGE;
	  break;
	case '>':
	  if (!context->dir_eof)
	    *ss=NEXT_PAGE;
	  break;
	}
    }
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
	case PREV_PAGE:
	  context->dir_pos -= 16;
	  ss=SELECT_FILE;
	  break;
	case NEXT_PAGE:
	  context->dir_pos += 16;
	  ss=SELECT_FILE;
	  break;
	}
    }
  
  return new_state;
}
