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

#define DIRECTORY_LIST_Y_OFFSET 3
#define DIRECTORY_LIST_SCREEN_WIDTH 36
#define DIRECTORY_LIST_ENTRIES_PER_PAGE 14

/**
 * Display directory entry
 */
bool diskulator_select_display_directory_entry(unsigned char i, char* entry, Context *context)
{
  if (entry[0]==0x7F)
    {
      context->dir_eof=true;
      return false; // End of directory
    }

  context->dir_eof=false;
  
  // Display filename
  screen_puts(2,DIRECTORY_LIST_Y_OFFSET+i,entry);

  // Display folder icon if directory.
  if (entry[strlen(entry)-1]=='/')
    screen_puts(0,DIRECTORY_LIST_Y_OFFSET+i,"\x04");

  return true;
}

/**
 * Clear directory page area
 */
void diskulator_select_display_clear_page(void)
{
  unsigned char i;

  for (i=2;i<19;i++)
    screen_clear_line(i);
}

/**
 * Display previous page
 */
void diskulator_select_display_prev_page(void)
{
  screen_puts(0,DIRECTORY_LIST_Y_OFFSET-1,"\x1C");
  screen_puts(2,DIRECTORY_LIST_Y_OFFSET-1,"<PREV PAGE>");
}

/**
 * Display previous page
 */
void diskulator_select_display_next_page(void)
{
  screen_puts(0,DIRECTORY_LIST_Y_OFFSET+DIRECTORY_LIST_ENTRIES_PER_PAGE,"\x1D");
  screen_puts(2,DIRECTORY_LIST_Y_OFFSET+DIRECTORY_LIST_ENTRIES_PER_PAGE,"<NEXT PAGE>");
}

/**
 * Display directory page
 */
void diskulator_select_display_directory_page(Context* context)
{
  char displayed_entry[DIRECTORY_LIST_SCREEN_WIDTH];
  unsigned char i;

  diskulator_select_display_clear_page();

  fuji_sio_open_directory(context->host_slot,context->directory);
  fuji_sio_set_directory_position(context->dir_page*DIRECTORY_LIST_ENTRIES_PER_PAGE);
  
  for (i=0;i<DIRECTORY_LIST_ENTRIES_PER_PAGE;i++)
    {
      fuji_sio_read_directory(displayed_entry,DIRECTORY_LIST_SCREEN_WIDTH);
      if (!diskulator_select_display_directory_entry(i,displayed_entry,context))
	break;
    }
    
  fuji_sio_close_directory(context->host_slot);
  context->entries_displayed=i;

  if (context->dir_page > 0)
    diskulator_select_display_prev_page();

  if (context->dir_eof == false)
    diskulator_select_display_next_page();
}

/**
 * Handle RETURN key - Select item.
 */
void diskulator_select_handle_return(unsigned char i, Context* context, SubState *ss)
{
  if ((i==0) && (context->dir_pos > 0))
    *ss = PREV_PAGE;
  else if (i==DIRECTORY_LIST_ENTRIES_PER_PAGE)
    *ss = NEXT_PAGE;
}

/**
 * Handle page nav
 */
void diskulator_select_handle_page_nav(unsigned char k, unsigned char i, Context *context, SubState *ss)
{
  switch(k)
    {
    case '-':
    case 0x1C:
      if (i==0 && context->dir_page > 0)
	*ss=PREV_PAGE;
      break;
    case '=':
    case 0x1D:
      if (i==DIRECTORY_LIST_ENTRIES_PER_PAGE-1 && context->dir_eof == false)
	*ss=NEXT_PAGE;
      break;
    }
}

/**
 * Select file
 */
void diskulator_select_select_file(Context* context, SubState* ss)
{
  unsigned char k=0;  // Key to process
  unsigned char i=0;  // cursor on page

  diskulator_select_display_directory_page(context);

  bar_show(DIRECTORY_LIST_Y_OFFSET+1);
  
  while (*ss==SELECT_FILE)
    {
      k=input_handle_key(); 
      diskulator_select_handle_page_nav(k,i,context,ss);     
      input_handle_nav_keys(k,DIRECTORY_LIST_Y_OFFSET+1,context->entries_displayed,&i);
      
      switch(k)
	{
	case 0x9B:
	  diskulator_select_handle_return(i,context,ss);
	  break;
	case '<':
	  if (context->dir_page > 0)
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
	  context->dir_page--;
	  ss=SELECT_FILE;
	  break;
	case NEXT_PAGE:
	  context->dir_page++;
	  ss=SELECT_FILE;
	  break;
	}
    }
  
  return new_state;
}
