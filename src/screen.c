/**
 * FujiNet Configuration Program
 *
 * Screen functions
 */

#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <peekpoke.h>
#include "screen.h"

unsigned char* video_ptr;
unsigned char* cursor_ptr;

void config_dlist=
  {
   DL_BLK8,
   DL_BLK8,
   DL_BLK8,
   DL_LMS(DL_CHR20x8x2),  //0,0x3
   DISPLAY_MEMORY,

   DL_CHR20x8x2,  //1,0x6
   DL_CHR40x8x1,  //2
   DL_CHR40x8x1,  //3
   DL_CHR40x8x1,  //4
   DL_CHR40x8x1,  //*5,0xa
   DL_CHR40x8x1,  //*6,0xb
   DL_CHR40x8x1,  //7
   DL_CHR40x8x1,  //8
   DL_CHR40x8x1,  //9
   DL_CHR40x8x1,  //*10,0x0f
   DL_CHR40x8x1,  //*11,0x10
   DL_CHR40x8x1,  //12
   DL_CHR40x8x1,  //13
   DL_CHR40x8x1,  //14
   DL_CHR40x8x1,  //15,0x14
   DL_CHR40x8x1,  //16
   DL_CHR40x8x1,  //17
   DL_CHR40x8x1,  //18
   DL_CHR40x8x1,  //19,0x18
   DL_CHR40x8x1,  //20
   DL_CHR40x8x1,  //21
   DL_CHR20x8x2,  //*22,0x1b
   DL_CHR20x8x2,  //*23,0x1c
   DL_JVB,	  //0x1d
   DISPLAY_LIST,  //0x1e,0x1f
   0,0,0,0
  };


void screen_clear()
{
  cursor_ptr=video_ptr;
  memset(video_ptr,0,GRAPHICS_0_SCREEN_SIZE);
}

void set_cursor(unsigned char x, unsigned char y)
{
  cursor_ptr=video_ptr+x+y*40;
}


void screen_clear_line(unsigned char y)
{
  set_cursor(0,y);
  memset(cursor_ptr,0,40);
}

/**
 * Print ATASCII string to display memory
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
  POKE(cursor_ptr++,c+offset);
}

void screen_append(char *s)
{
  while (*s!=0) 
    {     
      put_char(*s);
      ++s;
    }
}

void screen_puts(unsigned char x, unsigned char y, char *s)
{
  set_cursor(x,y);
  screen_append(s);
}

/**
 * Input a string at x,y
 */
int _screen_input(unsigned char x, unsigned char y, char* s, unsigned char maxlen)
{
  unsigned char k,o;
  unsigned char * input_start_ptr;
  
  o=strlen(s);
  set_cursor(x,y);
  input_start_ptr=cursor_ptr;
  screen_append(s);

  POKE(cursor_ptr,0x80); // turn on cursor

  do
    {
      k=cgetc();

      if ( k== KCODE_ESCAPE ) // ESC key to cancel
        return -1;

      if (k==KCODE_BACKSP) // backspace
	{
	  if (cursor_ptr>input_start_ptr)
            {
	      s[--o]=0;
	      POKEW(--cursor_ptr,0x0080);
            }
	}
      else if ((k>0x1F) && (k<0x80)) // printable ascii
	{
	  if (o<maxlen-1)
            {
	      put_char(k);
	      s[o++]=k;
	      POKE(cursor_ptr,0x80);
            }
	}
    } while (k!=KCODE_RETURN);
  POKE(cursor_ptr,0x00); // clear cursor
}

/**
 * Print IP address at position
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
 * Special hex output of numbers under 16, e.g. 9 -> 09, 10 -> 0A
 */
void itoa_hex(unsigned char val, char *buf)
{

  if (val < 16) {
    *(buf++) = '0';
  }
  itoa(val, buf, 16);

}

/**
 * Print MAC address at position
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
 * Patch the dlist for the hosts screen
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
  // the same as above
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

void screen_init()
{
  memcpy((void *)DISPLAY_LIST,&config_dlist,sizeof(config_dlist)); // copy display list to $0600
  OS.sdlst=(void *)DISPLAY_LIST;                     // and use it.
  video_ptr=(unsigned char*)(DISPLAY_MEMORY);
}
