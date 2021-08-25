/**
 * FujiNet Configuration Program: screen functions 
 * The screen functions are a common set of graphics and text string functions to display information to the video output device.  
 * These screen functions is comprised of two files; screen.c and screen.h.  Screen.c sets up the display dimensions, memory and 
 * initializes the display as well as provide various text manipulations functions for proper display.  The screen.h file include
 * defines for various items such as keyboard codes, functions and screen memory addresses.
 *
 **/

 
 #include <stdlib.h>        // Used for memory handling routines
 #include <string.h>        // Used for maninpulating strings and characters
 #include <conio.h>         // Used for interacting with the standard Atari 'console' 
 #include <peekpoke.h>      // Atari memory handling
 #include "screen.h"        // Local header file for the screen defines 

 unsigned char* video_ptr;  // a pointer to the memory address containing the screen contents
 unsigned char* cursor_ptr;  // a pointer to the current cursor position on the screen

 // Setup the screen display dimensions for the program to be able to draw items on the screen
void config_dlist=
  {
   DL_BLK8,                 // The first three scan lines of the display are normally left blank to compensate for old CRT overscan 
   DL_BLK8,                 // Line 2 of 3
   DL_BLK8,                 // Line 3 of 3
   DL_LMS(DL_CHR20x8x2),    // The following two lines set the memory lcoation to which the atari looks up the location of the display list.  0,0x3
   DISPLAY_MEMORY,          // This is the high order bit location of the display list.  Defined in screen.h 

   DL_CHR20x8x2,            // First line of displayable content.  1,0x6
   DL_CHR40x8x1,            // Line 2
   DL_CHR40x8x1,            // Line 3
   DL_CHR40x8x1,            // Line 4
   DL_CHR40x8x1,            // Line *5,0xa
   DL_CHR40x8x1,            // *6,0xb
   DL_CHR40x8x1,            // 7
   DL_CHR40x8x1,            // 8
   DL_CHR40x8x1,            // 9
   DL_CHR40x8x1,            // *10,0x0f
   DL_CHR40x8x1,            // *11,0x10
   DL_CHR40x8x1,            // 12
   DL_CHR40x8x1,            // 13
   DL_CHR40x8x1,            // 14
   DL_CHR40x8x1,            // 15,0x14
   DL_CHR40x8x1,            // 16
   DL_CHR40x8x1,            // 17
   DL_CHR40x8x1,            // 18
   DL_CHR40x8x1,            // 19,0x18
   DL_CHR40x8x1,            // 20
   DL_CHR40x8x1,            // 21
   DL_CHR20x8x2,            // *22,0x1b
   DL_CHR20x8x2,            // *23,0x1c
   DL_JVB,	                // Signal to ANTIC end of DISPLAY_LIST has been reached and loop back to the begining.  The jump to the begining is located at the next two bits defined below.
   DISPLAY_LIST,            // Memory address containing the entire display list.  0x1e,0x1f
   0,0,0,0                  // Reserved for future extension to DISPLAY_LIST
  };

/*********************
 * Function to clear the atari screen of anything that is currently being displayed
 *
 **/
void screen_clear()
{
  cursor_ptr=video_ptr;     // Assign the current position of the cursor position address to the address of the video screen
  memset(video_ptr,0,GRAPHICS_0_SCREEN_SIZE);   // Fill the memory address with blanks of the size of the screen; in this case Atari graphics mode 0 size.
}

/*********************
 * Function to set the cursor position on the screen.
 *
 **/
void set_cursor(unsigned char x, unsigned char y)
{
  cursor_ptr=video_ptr+x+y*40;  // Set the pointer to the cursor at the address of x,y.  Adding 40 as the first 40 positions are line 1?
}

/*********************
 * Function to clear a single line at position y on the screen.
 *
 **/
void screen_clear_line(unsigned char y)
{
  set_cursor(0,y);          // move the cursor X position 0 and Y position.
  memset(cursor_ptr,0,40);  // fill the memory with spaces.  The zeros in the memory addresses are interpreted as spaces on the console
}


/**********************
 * Print ATASCII string to display memory.  Note: ATASCII is not a 1:1 mapping for ASCII.  It is a ven diagram with significant overlap.  
 */
void put_char(char c)
{
  char offset;
  if (c < 32)
    {
      offset=64;
    }
  else if (c<96)
    {
      offset=-32;
    }
  else
    {
      offset=0;
    }
  POKE(cursor_ptr++,c+offset);  // Insert into the locaiton in memory for next bit in cursor_ptr the ATASCI character.  c+offset is the ATASCI character desired to be displayed. 
}

/**********************
 * Print a character on the screen and increment through the input buffer until empty  
 **/
void screen_append(char *s)
{
  while (*s!=0) 
    {     
      put_char(*s);
      ++s;
    }
}

/**********************
 * Position the cursor at x and y and then output the characters in the input buffer at the position 
 **/
void screen_puts(unsigned char x, unsigned char y, char *s)
{
  set_cursor(x,y);
  screen_append(s);
}

/***********************
 * Capture keyboard input a string at x,y
 */
int _screen_input(unsigned char x, unsigned char y, char* s, unsigned char maxlen)
{
  unsigned char k,o;
  unsigned char * input_start_ptr;
  
  o=strlen(s);              // assign to local var the size of s which contains the current string
  set_cursor(x,y);          // move the cusor to coordinates x,y
  input_start_ptr=cursor_ptr;//assign the value currently in cursor_ptr to local var input_start
  screen_append(s);         // call screen_append function and pass by value (a copy) the contents of s

  POKE(cursor_ptr,0x80);    // turn on cursor

  // Start capturing the keyboard input into local var k
  do
    {
      k=cgetc();            // Capture keyboard input into k

      if ( k== KCODE_ESCAPE ) // KCODE_ESCAPE is the ATASCI code for the escape key which is commonly used to cancel.  
        return -1;

      if (k==KCODE_BACKSP)  // KCODE_BACKSP is the ATASCI backspace key.  This if clause test for backspace and updates cursor_ptr to the remainder of contents upto the last backspace
	{
	  if (cursor_ptr>input_start_ptr)   // execute only if cursor_ptr is greater than input_start_ptr
            {
	      s[--o]=0;         // 
	      POKEW(--cursor_ptr,0x0080);   // move the last bit of the cursor_ptr back one and write the location of the cursor_ptr contents to user zero page address 0x80 
            }
	}
      else if ((k>0x1F) && (k<0x80))    // Display printable ascii to screen
	{
	  if (o<maxlen-1)
            {
	      put_char(k);
	      s[o++]=k;
	      POKE(cursor_ptr,0x80);
            }
	}
    } while (k!=KCODE_RETURN);  // Continue to capture keyboard input until return (0x9B)
  POKE(cursor_ptr,0x00); // clear cursor
}

/**
 * Convert integer to a string and print IP address at position x,y 
 * 
 */
void screen_print_ip(unsigned char x, unsigned char y, unsigned char *buf)
{
  unsigned char i = 0;
  unsigned char tmp[4];

  set_cursor(x,y);
  for (i = 0; i < 4; i++)
    {
      itoa(buf[i], tmp, 10);
      screen_append(tmp);
      if (i == 3) break;
      screen_append(".");
    }
}

/**
 * Convert hex to a string.  Special hex output of numbers under 16, e.g. 9 -> 09, 10 -> 0A
 */
void itoa_hex(unsigned char val, char *buf)
{

  if (val < 16) {
    *(buf++) = '0';
  }
  itoa(val, buf, 16);

}

/**
 * Convert hex to a string and print as a MAC address at position x, y
 */
void screen_print_mac(unsigned char x, unsigned char y, unsigned char *buf)
{
  unsigned char i = 0;
  unsigned char tmp[3];

  set_cursor(x,y);
  for (i = 0; i < 6; i++)
    {
      itoa_hex(buf[i], tmp);
      screen_append(tmp);
      if (i == 5) break;
      screen_append(":");
    }
}
/**
 * A set of functions to support custom screen formating to support different sections in the config program.
 * The Patch the dlist for the hosts screen
 */
void screen_dlist_diskulator_hosts(void)
{
  POKE(DISPLAY_LIST+0x0f,6);
  POKE(DISPLAY_LIST+0x10,6);
  POKE(DISPLAY_LIST+0x0a,2);
  POKE(DISPLAY_LIST+0x0b,2);
  POKE(DISPLAY_LIST+0x1b,2);
  POKE(DISPLAY_LIST+0x1c,2);
}

/**
 * Patch the dlist for the info screen
 */
void screen_dlist_diskulator_info(void)
{
  POKE(DISPLAY_LIST+0x0a,7);
  POKE(DISPLAY_LIST+0x0b,6);
  POKE(DISPLAY_LIST+0x0f,2);
  POKE(DISPLAY_LIST+0x10,2);
}

/**
 * Patch the dlist for wifi screens
 */
void screen_dlist_wifi(void)
{
  POKE (DISPLAY_LIST+0x0a,2);
  POKE (DISPLAY_LIST+0x0b,2);
  POKE (DISPLAY_LIST+0x1b,6);
  POKE (DISPLAY_LIST+0x1c,6);
}

/**
 * Patch the dlist for mount and boot screen.
 */
void screen_dlist_mount_and_boot(void)
{
  // the display setup is the same as screen_dlist_wifi()
  screen_dlist_wifi();
}

/**
 * Patch the dlist for the diskulator select screen.
 */
void screen_dlist_diskulator_select(void)
{
  POKE(DISPLAY_LIST+0x0f,2);
  POKE(DISPLAY_LIST+0x10,2);
  POKE(DISPLAY_LIST+0x1b,2);
  POKE(DISPLAY_LIST+0x1c,2);
}

/**
 * Patch the dlist for the diskulator select screen.
 */
void screen_dlist_diskulator_select_aux(void)
{
  screen_dlist_diskulator_select();
}

/**
 * Patch the dlist for the diskulator slot screen.
 */
void screen_dlist_diskulator_slot(void)
{
  screen_dlist_diskulator_select();
}

/**
 * Patch the dlist for the diskulator copy host slot screen.
 */
void screen_dlist_diskulator_copy_destination_host_slot(void)
{

}
/**
* Initialize the display using the configuration in DISPLAY_LIST
*/
void screen_init()
{
  memcpy((void *)DISPLAY_LIST,&config_dlist,sizeof(config_dlist)); // copy display list to $0600
  OS.sdlst=(void *)DISPLAY_LIST;                    // and use it.
  video_ptr=(unsigned char*)(DISPLAY_MEMORY);       // assign the value of DISPLAY_MEMORY to video_ptr
}

/*********************
   * References 
   * 1. Display List (website): http://gury.atari8.info/ref/dl.php
   * 2. Display List Simplified (article): https://www.atarimagazines.com/vbook/displaylistssimplified.html
   * 3. Mapping the Atari (book) : https://www.atariarchives.org/mapping/appendix8.php (appendix 8)
   * 4. ATASCI : https://en.wikipedia.org/wiki/ATASCII
   **/