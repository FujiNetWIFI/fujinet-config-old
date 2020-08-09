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
#include "error.h"

typedef enum _substate
  {
   SELECT_FILE,
   PREV_PAGE,
   NEXT_PAGE,
   ADVANCE_DIR,
   DEVANCE_DIR,
   DONE
  } SubState;

typedef unsigned short Page;
typedef unsigned char PageOffset;

#define DIRECTORY_LIST_Y_OFFSET 3
#define DIRECTORY_LIST_SCREEN_WIDTH 36
#define DIRECTORY_LIST_FULL_WIDTH 128
#define DIRECTORY_LIST_ENTRIES_PER_PAGE 14

/**
 * Display directory path
 */
void diskulator_select_display_directory_path(Context* context)
{
  screen_clear_line(1);
  screen_puts(0,1,context->directory);
}

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
  screen_puts(0,DIRECTORY_LIST_Y_OFFSET-1,"\x9C");
  screen_puts(2,DIRECTORY_LIST_Y_OFFSET-1,"<PREV PAGE>");
}

/**
 * Display previous page
 */
void diskulator_select_display_next_page(void)
{
  screen_puts(0,DIRECTORY_LIST_Y_OFFSET+DIRECTORY_LIST_ENTRIES_PER_PAGE,"\x9E");
  screen_puts(2,DIRECTORY_LIST_Y_OFFSET+DIRECTORY_LIST_ENTRIES_PER_PAGE,"<NEXT PAGE>");
}

/**
 * Display directory page
 */
void diskulator_select_display_directory_page(Context* context)
{
  char displayed_entry[DIRECTORY_LIST_SCREEN_WIDTH];
  unsigned char i;

  bar_clear();
  
  diskulator_select_display_clear_page();

  fuji_sio_open_directory(context->host_slot,context->directory);
  if (fuji_sio_error())
    error_fatal(ERROR_OPENING_DIRECTORY);
  
  fuji_sio_set_directory_position(context->dir_page*DIRECTORY_LIST_ENTRIES_PER_PAGE);
  if (fuji_sio_error())
    error_fatal(ERROR_SETTING_DIRECTORY_POSITION);
  
  for (i=0;i<DIRECTORY_LIST_ENTRIES_PER_PAGE;i++)
    {
      fuji_sio_read_directory(displayed_entry,DIRECTORY_LIST_SCREEN_WIDTH);
      if (fuji_sio_error())
	error_fatal(ERROR_READING_DIRECTORY);
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
  fuji_sio_open_directory(context->host_slot,context->directory);
  if (fuji_sio_error())
    error_fatal(ERROR_OPENING_DIRECTORY);
  
  fuji_sio_set_directory_position((context->dir_page*DIRECTORY_LIST_ENTRIES_PER_PAGE)+i);
  if (fuji_sio_error())
    error_fatal(ERROR_SETTING_DIRECTORY_POSITION);

  fuji_sio_read_directory(context->filename,DIRECTORY_LIST_FULL_WIDTH);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_DIRECTORY);

  // Handle if this is a directory
  if (context->filename[strlen(context->filename)-1]=='/')
    {
      strcat(context->directory,context->filename);
      memset(context->filename,0,sizeof(context->filename));
      diskulator_select_display_directory_path(context);
      *ss=ADVANCE_DIR; // Stay here, go to the new directory.
    }
  else
    {
      *ss=DONE; // We are done with the select screen.
      context->state=DISKULATOR_SLOT;
    }
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
 * Devance (move up) directory
 */
void diskulator_select_devance_directory(Context* context)
{
  unsigned char i=strlen(context->directory)-2; // skip over the last '/'

  while (context->directory[i--]!='/')
    {
      // Skip over it.
    }

  // i is now at next /, truncate it.
  i++;
  context->directory[++i]=0;
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
	case 0x1B:
	  *ss=DONE;
	  context->state=DISKULATOR_HOSTS;
	  break;
	case 0x7E:
	  *ss=DEVANCE_DIR;
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

  memset(context->filename,0,sizeof(context->filename));
  memset(context->directory,0,sizeof(context->directory));
  strcpy(context->directory,"/");
  
  screen_puts(4, 0, "DISK IMAGES");
  
  screen_puts(0, 21, "\xD9\xB2\xA5\xB4\xB5\xB2\xAE\x19"
	      "PICK \xD9\xA5\xB3\xA3\x19"
	      "ABORT");

  diskulator_select_display_directory_path(context);
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
	case ADVANCE_DIR:
	  context->dir_page=0;
	  context->dir_eof=false;
	  ss=SELECT_FILE;
	  break;
	case DEVANCE_DIR:
	  diskulator_select_devance_directory(context);
	  diskulator_select_display_directory_path(context);
	  context->dir_page=0;
	  context->dir_eof=false;
	  ss=SELECT_FILE;
	  break;
	}
    }
  
  return new_state;
}
