/**
 * #FUJINET CONFIG
 * Diskulator Select Disk image screen
 */

#include <atari.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "diskulator_select.h"
#include "diskulator_copy.h"
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

extern char text_empty[];

#define DIRECTORY_LIST_HOSTNAME_Y 1
#define DIRECTORY_LIST_DIRPATH_Y 2
#define DIRECTORY_LIST_Y_OFFSET 4 /* N.B., "Previous" pagination entry will appear 1 line above the list */
#define DIRECTORY_LIST_ENTRIES_PER_PAGE 13 /* N.B. "Next" pagination entry will appear 1 line below the list */
#define DIRECTORY_LIST_FULLFILENAME_Y 18
#define DIRECTORY_LIST_FULLFILENAME_HEIGHT 3
#define DIRECTORY_LIST_SCREEN_WIDTH 36
#define DIRECTORY_LIST_FULL_WIDTH 128
#define DIRECTORY_LIST_SHOW_FULL_FILENAME_DELAY 24

bool files_present=false;

/**
 * Clear full filename area
 */
void diskulator_select_clear_file_area(void)
{
  int i;
  for (i = DIRECTORY_LIST_FULLFILENAME_Y; i < DIRECTORY_LIST_FULLFILENAME_Y + DIRECTORY_LIST_FULLFILENAME_HEIGHT; i++) {
    screen_clear_line(i);
  }
}

/**
 * Display directory path (and "SD" or TNFS server name)
 */
void diskulator_select_display_directory_path(Context* context)
{
  screen_clear_line(DIRECTORY_LIST_HOSTNAME_Y);
  screen_append(context->host);

  screen_clear_line(DIRECTORY_LIST_DIRPATH_Y);
  screen_append(context->directory);
}

/**
 * Display directory entry
 */
bool diskulator_select_display_directory_entry(unsigned char i, char* entry, Context *context)
{
  files_present=true;
  
  if (entry[0]==0x7F)
    {
      context->dir_eof=true;
      return false; // End of directory
    }

  context->dir_eof=false;

  // Display filename
  if (entry[0]=='+')
    screen_puts(2,DIRECTORY_LIST_Y_OFFSET+i,entry+1);
  else
    screen_puts(2,DIRECTORY_LIST_Y_OFFSET+i,entry);

  // Display folder icon if directory.
  if (entry[strlen(entry)-1]=='/')
    screen_puts(0,DIRECTORY_LIST_Y_OFFSET+i,CH_FOLDER);
  else if (entry[0]=='+') 
    screen_puts(0,DIRECTORY_LIST_Y_OFFSET+i,CH_SERVER);
 

  return true;
}

/**
 * Clear directory page area
 */
void diskulator_select_display_clear_page(void)
{
  unsigned char i;

  screen_dlist_diskulator_select();

  for (i = DIRECTORY_LIST_Y_OFFSET; i < DIRECTORY_LIST_Y_OFFSET + DIRECTORY_LIST_ENTRIES_PER_PAGE; i++) {
    screen_clear_line(i);
  }

  diskulator_select_clear_file_area();
}

/**
 * Display previous page
 */
void diskulator_select_display_prev_page(void)
{
  screen_puts(0,DIRECTORY_LIST_Y_OFFSET-1,CH_KEY_LT"Previous Page");
}

/**
 * Display previous page
 */
void diskulator_select_display_next_page(void)
{
  screen_puts(0,DIRECTORY_LIST_Y_OFFSET+DIRECTORY_LIST_ENTRIES_PER_PAGE,CH_KEY_GT"Next Page");
}

/**
 * Display current filter
 */
#define FILTER_LABEL "Filter: "
void diskulator_select_display_filter(Context *context)
{
  char filter_str[sizeof(FILTER_LABEL) + sizeof(context->filter) + 1] = FILTER_LABEL;

  strcat(filter_str,context->filter);
  /* TODO: 14 to make sure it doesn't overlap "Previous" pagination;
     should probably be on its own line, to fit all 32 allowed characters */
  screen_puts(14,DIRECTORY_LIST_Y_OFFSET-1,filter_str);
}

/**
 * Display directory page
 */
void diskulator_select_display_directory_page(Context* context)
{
  char displayed_entry[DIRECTORY_LIST_SCREEN_WIDTH];
  unsigned char i;
  unsigned short pos;
  unsigned char retry=5;

  bar_clear();

  diskulator_select_display_clear_page();

  while (retry>0)
    {
      if (context->filter[0]!=0x00)
        {
          diskulator_select_display_filter(context);
          memset(context->directory_plus_filter,0,sizeof(context->directory_plus_filter));
          strcpy(context->directory_plus_filter,context->directory);
          strcpy(&context->directory_plus_filter[strlen(context->directory_plus_filter)+1],context->filter);
          fuji_sio_open_directory(context->host_slot,context->directory_plus_filter);
        }
      else
        fuji_sio_open_directory(context->host_slot,context->directory);

      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error())
    {
      error(ERROR_OPENING_DIRECTORY);
      wait_a_moment();
      context->state=CONNECT_WIFI;
      return;
    }

  pos=context->dir_page*DIRECTORY_LIST_ENTRIES_PER_PAGE;

  retry=5;

  while (retry>0)
    {
      fuji_sio_set_directory_position(pos);

      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error() && pos!=0)
    {
      error(ERROR_SETTING_DIRECTORY_POSITION);
      goto exit_error;
    }

  for (i=0;i<DIRECTORY_LIST_ENTRIES_PER_PAGE;i++)
    {
      retry=5;

      while (retry>0)
        {
          fuji_sio_read_directory(displayed_entry,DIRECTORY_LIST_SCREEN_WIDTH);

          if (fuji_sio_error())
            retry--;
          else
            break;
        }

      if (fuji_sio_error())
        {
          error(ERROR_READING_DIRECTORY);
	  goto exit_error;
        }

      context->entry_widths[i]=strlen(displayed_entry);

      if (!diskulator_select_display_directory_entry(i,displayed_entry,context))
        break;
    }

  fuji_sio_close_directory(context->host_slot);
  context->entries_displayed=i;

  if (context->dir_page > 0)
    diskulator_select_display_prev_page();

  if (context->dir_eof == false)
    diskulator_select_display_next_page();

  if (i==0 && context->dir_page==0)
    {
      files_present = false;
      screen_puts(2,DIRECTORY_LIST_Y_OFFSET,"--Empty--");
    }
  
  return;

exit_error:

  wait_a_moment();
  context->state=DISKULATOR_HOSTS;
  return;

}

/**
 * Handle RETURN key - Select item.
 */
void diskulator_select_handle_return(unsigned char i, Context* context, SubState *ss)
{
  unsigned short pos;
  unsigned char retry=5;

  if (context->entries_displayed==0)
    {
      if (context->copySubState == SELECT_DESTINATION_FOLDER)
	return;
      else
	{
	  *ss=DONE;
	  context->state=DISKULATOR_COPY;
	}
    }
  while (retry>0)
    {
      if (context->filter[0]==0x00)
        fuji_sio_open_directory(context->host_slot,context->directory);
      else
        fuji_sio_open_directory(context->host_slot,context->directory_plus_filter);

      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error())
    {
      error(ERROR_OPENING_DIRECTORY);
      wait_a_moment();
      context->state=DISKULATOR_HOSTS;
      return;
    }

  pos = context->dir_page*DIRECTORY_LIST_ENTRIES_PER_PAGE+i;

  retry = 5;

  while (retry>0)
    {
      fuji_sio_set_directory_position(pos);
      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error() && pos != 0)
    {
      error(ERROR_SETTING_DIRECTORY_POSITION);
      wait_a_moment();
      context->state=DISKULATOR_HOSTS;
      return;
    }

  retry = 5;

  while (retry>0)
    {
      fuji_sio_read_directory(context->filename,DIRECTORY_LIST_FULL_WIDTH);
      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error())
    {
      error(ERROR_READING_DIRECTORY);
      wait_a_moment();
      context->state=DISKULATOR_HOSTS;
      return;
    }

  fuji_sio_close_directory(context->host_slot);

  // Handle if this is a directory
  if (context->filename[strlen(context->filename)-1]=='/')
    {
      if (context->copySubState == SELECT_HOST_SLOT)
	{
	  // Silently fail for now
	}
      else
	{
	  strcat(context->directory,context->filename);
	  memset(context->filename,0,sizeof(context->filename));
	  diskulator_select_display_directory_path(context);
	  *ss=ADVANCE_DIR; // Stay here, go to the new directory.
	}
    }
  // Handle if this is a new host.
  else if (context->filename[0]=='+')
    { 
	  // set slot index to 7 and copy new host to hostSlots
	  strcpy(context->hostSlots.host[7], context->filename+1);
	  context->host_slot = 7;
      fuji_sio_write_host_slots(&context->hostSlots);
	  
	  // mount the new host.
	  context->state=DISKULATOR_SELECT;
	  fuji_sio_mount_host(context->host_slot,&context->hostSlots);
	  
	  if (fuji_sio_error())
		{
		  error(ERROR_MOUNTING_HOST_SLOT);
		  wait_a_moment();
		  context->state=CONNECT_WIFI;
		}

	  *ss=DONE;
    }
  else
    {
      if (context->copySubState == DISABLED)
	{
	  *ss=DONE; // We are done with the select screen.
	  context->state=DISKULATOR_SLOT;
	}
      else
	{
	  *ss=DONE;
	  context->state=DISKULATOR_COPY;
	}
    }
}

/**
 * Handle page nav
 */
void diskulator_select_handle_page_nav(unsigned char k, unsigned char i, Context *context, SubState *ss)
{
  if (context->entries_displayed==0)
    return;

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

  while (i<255)
    context->directory[++i]=0;
}

/**
 * New Disk
 */
void diskulator_select_new_disk(Context* context, SubState* ss)
{
  char tmp_str[8];

  screen_clear_line(20);
  screen_clear_line(21);
  screen_dlist_diskulator_select_aux();

  memset(context->filename,0,sizeof(context->filename));
  screen_puts(0,20,"Enter name of new disk image file");
  screen_input(0,21,context->filename);

  if (context->filename[0]==0x00)
    {
      *ss=ADVANCE_DIR;
      diskulator_select_setup(context); // Reset screen.
      screen_clear_line(20);
      return;
    }

  screen_clear_line(20);
  screen_clear_line(21);

  screen_puts(0, 20, "Size?"
              CH_KEY_LABEL_L CH_INV_1 CH_KEY_LABEL_R "90K  "
              CH_KEY_LABEL_L CH_INV_2 CH_KEY_LABEL_R "130K  "
              CH_KEY_LABEL_L CH_INV_3 CH_KEY_LABEL_R "180K  "
              CH_KEY_LABEL_L CH_INV_4 CH_KEY_LABEL_R "360K  ");
  screen_puts(5, 21,
              CH_KEY_LABEL_L CH_INV_5 CH_KEY_LABEL_R "720K "
              CH_KEY_LABEL_L CH_INV_6 CH_KEY_LABEL_R "1440K "
              CH_KEY_LABEL_L CH_INV_7 CH_KEY_LABEL_R "Custom ?");

  memset(tmp_str,0,sizeof(tmp_str));
  screen_input(34,21,tmp_str);

  switch(tmp_str[0])
    {
    case '1':
      context->newDisk_ns=720;
      context->newDisk_sz=128;
      break;
    case '2':
      context->newDisk_ns=1040;
      context->newDisk_sz=128;
      break;
    case '3':
      context->newDisk_ns=720;
      context->newDisk_sz=256;
      break;
    case '4':
      context->newDisk_ns=1440;
      context->newDisk_sz=256;
      break;
    case '5':
      context->newDisk_ns=2880;
      context->newDisk_sz=256;
      break;
    case '6':
      context->newDisk_ns=5760;
      context->newDisk_sz=256;
      break;
    case '7':
      screen_clear_line(20);
      screen_clear_line(21);

      memset(tmp_str,0,sizeof(tmp_str));
      screen_puts(0,20,"# Sectors?");
      screen_input(11,20,tmp_str);
      context->newDisk_ns=atoi(tmp_str);

      memset(tmp_str,0,sizeof(tmp_str));
      while (tmp_str[0]!='1' && tmp_str[0]!='2' && tmp_str[0]!='5')
      {
        screen_puts(0,21,"Sector Size (128/256/512)?");
        screen_input(27,21,tmp_str);
      }
      switch (tmp_str[0])
      {
        case '1': context->newDisk_sz=128;
        case '2': context->newDisk_sz=256;
        case '5': context->newDisk_sz=512;
      }
      break;
    }

  memset(tmp_str,0,sizeof(tmp_str));
  screen_clear_line(20);
  screen_clear_line(21);
  screen_puts(0,20,"Are you sure (Y/N)?");
  screen_input(20,20,tmp_str);

  if (tmp_str[0]=='Y' || tmp_str[0]=='y')
    {
      *ss=DONE;
      context->state=DISKULATOR_SLOT;
      context->newDisk=true;
    }
}

/**
 * Set filter
 */
void diskulator_select_set_filter(Context *context, SubState *ss)
{
  diskulator_select_display_filter(context);
  screen_input(22,DIRECTORY_LIST_Y_OFFSET-1,context->filter);
  *ss=ADVANCE_DIR;
}

/**
 * Show full filename
 */
void diskulator_select_show_full_filename(Context *context, unsigned char i)
{
  unsigned char retry=5;

  while (retry>0)
    {
      if (context->filter[0]!=0x00)
        fuji_sio_open_directory(context->host_slot,context->directory_plus_filter);
      else
        fuji_sio_open_directory(context->host_slot,context->directory);

      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error())
    {
      error(ERROR_OPENING_DIRECTORY);
      goto exit_error;
    }

  retry=5;

  while (retry>0)
    {
      fuji_sio_set_directory_position((context->dir_page*DIRECTORY_LIST_ENTRIES_PER_PAGE)+i);

      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error())
    {
      error(ERROR_SETTING_DIRECTORY_POSITION);
      goto exit_error;
    }

  retry=5;

  while (retry>0)
    {
      fuji_sio_read_directory(context->filename,DIRECTORY_LIST_FULL_WIDTH);

      if (fuji_sio_error())
        retry--;
      else
        break;
    }

  if (fuji_sio_error())
    {
      error(ERROR_READING_DIRECTORY);
      goto exit_error;
    }

  fuji_sio_close_directory(context->host_slot);

  screen_clear_line(18);
  screen_clear_line(19);
  screen_clear_line(20);

  screen_puts(0,18,context->filename);
  return;
exit_error:

      wait_a_moment();
      context->state=DISKULATOR_HOSTS;
      return;
}

/**
 * Select file
 */
void diskulator_select_select_file(Context* context, SubState* ss)
{
  unsigned char k=0;  // Key to process
  unsigned char i=0;  // cursor on page
  bool long_filename_displayed=false;

  diskulator_select_display_directory_page(context);

  if (context->state != DISKULATOR_SELECT)
  {
	*ss=DONE;
	return;
  }

  bar_show(DIRECTORY_LIST_Y_OFFSET+1);

  while (*ss==SELECT_FILE)
    {
      k=input_handle_key_ucase();
      diskulator_select_handle_page_nav(k,i,context,ss);

      if (context->entries_displayed>0)
        input_handle_nav_keys(k,DIRECTORY_LIST_Y_OFFSET+1,context->entries_displayed,&i);

      if (input_handle_console_keys() == 0x03)
        {
          *ss=DONE;
          context->state = MOUNT_AND_BOOT;
        }

      // Clear file area if we move cursor
      if (k>0)
        diskulator_select_clear_file_area();

      // See if we need to display a long filename
      if ((context->entry_widths[i]>DIRECTORY_LIST_SCREEN_WIDTH-2) &&
          (OS.rtclok[2]>DIRECTORY_LIST_SHOW_FULL_FILENAME_DELAY) &&
          (long_filename_displayed==false))
        {
          diskulator_select_clear_file_area();
          diskulator_select_show_full_filename(context,i);
          long_filename_displayed=true;
        }

      switch(k)
        {
        case '=':
        case '-':
        case 0x1C:
        case 0x1D:
          long_filename_displayed=false;
          break;
        case '*': // right arrow
        case KCODE_RETURN:
	  if (files_present==true)
	    diskulator_select_handle_return(i,context,ss);
          break;
        case '!': //joy left + fire
          *ss=DONE;
          context->state = MOUNT_AND_BOOT;
          break;
        case '<':
          if (context->dir_page > 0)
            *ss=PREV_PAGE;
          else
            *ss=DEVANCE_DIR;
          break;
        case '>':
          if (!context->dir_eof)
            *ss=NEXT_PAGE;
          break;
        case KCODE_ESCAPE:
        case '^': // joy up + fire
          *ss=DONE;
          context->dir_page=0;
          context->state=DISKULATOR_HOSTS;
          break;
        case KCODE_BACKSP:
        case '+': // left arrow
          *ss=DEVANCE_DIR;
          context->dir_page=0;
          break;
        case 'B':
          *ss=DONE;
          context->state = MOUNT_AND_BOOT;
          break;
        case 'C':
          context->state = DISKULATOR_COPY;
          context->copySubState = context->copySubState == DISABLED?
            SELECT_HOST_SLOT:DO_COPY;
          diskulator_select_handle_return(i,context,ss);
          break;
        case 'N':
          diskulator_select_new_disk(context,ss);
          break;
        case 'F':
          diskulator_select_set_filter(context,ss);
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

  if (context->directory[0]==0x00)
    strcpy(context->directory,"/");

  context->newDisk = false;

  screen_puts(4, 0, "DISK IMAGES");

  screen_puts(0,21,
        CH_KEY_LEFT CH_KEY_DELETE "Up Dir"
        CH_KEY_N "ew"
        CH_KEY_F "ilter");

  if (context->copySubState == SELECT_DESTINATION_FOLDER)
    {
      screen_puts(30,21, CH_KEY_C "Do It!");
    }
  else
    {
      screen_puts(30, 21, CH_KEY_C "opy");
    }
  
  screen_puts(0,22,
      CH_KEY_RIGHT CH_KEY_RETURN "Choose"
      CH_KEY_OPTION "Boot"
      CH_KEY_ESC "Abort");
  diskulator_select_display_directory_path(context);
}

/**
 * Diskulator select disk image
 */
State diskulator_select(Context *context)
{
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

  return context->state;
}
