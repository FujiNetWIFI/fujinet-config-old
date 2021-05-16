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
  memset(video_ptr,0,GRAPHICS_0_SCREEN_SIZE);
}

void screen_clear_line(unsigned char y)
{
  memset(&video_ptr[(y)*40],0,40);
}

/**
 * Print ATASCII string to display memory
 */
void screen_puts(unsigned char x, unsigned char y, char *s)
{
  char offset;
  
  do
    {     
      if (*s < 32)
	{
	  offset=64;
	}
      else if (*s<96)
	{
	  offset=-32;
	}
      else
	{
	  offset=0;
	}
      
      SetChar(x++,y,*s+offset);

      ++s;
      
    } while(*s!=0);
}

/**
 * Input a string at x,y
 */
int _screen_input(unsigned char x, unsigned char y, char* s, unsigned char maxlen)
{
  unsigned char c,k,o;
  unsigned char outc[2]={0,0};
  unsigned char q;
  
  o=strlen(s);
  c=x+o;

  SetChar(c+1,y,0x80); // turn on cursor

  k=0;
  
  while (k!=155)
    {
      k=cgetc();

      if ( k== 0x1B ) // ESC key to cancel
        return -1;

      if (k==0x7E) // backspace
	{
	  if (c>x)
            {
	      SetChar(c+1,y,0);
	      s[--o]=0;
	      --c;
	      SetChar(c+1,y,0x80);
            }
	}
      else if (k==0x9b) // return (EOL)
	{
	  SetChar(c+1,y,GetChar(c+1,y)&0x7F);
	  // exit while.
	}
      else if ((k>0x1F) && (k<0x80)) // printable ascii
	{
	  if (o<maxlen-1)
            {
	      SetChar(c+1,y,GetChar(c+1,y)&0x7F);
	      outc[0]=k;
	      screen_puts(c+1,y,outc);
	      SetChar(c+2,y,0x80);
	      s[o++]=k;
	      c++;
            }
	}
    }
}

/**
 * Print IP address at position
 */
void screen_print_ip(unsigned char x, unsigned char y, unsigned char *buf)
{
    unsigned char i = 0;
    unsigned char o = 0;
    unsigned char tmp[4];

    for (i = 0; i < 4; i++)
    {
        itoa(buf[i], tmp, 10);
        screen_puts(x + o, y, tmp);
        o += strlen(tmp);
        if (i < 3)
            screen_puts(x + (o++), y, ".");
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
    unsigned char o = 0;
    unsigned char tmp[3];

    for (i = 0; i < 6; i++)
    {
        itoa_hex(buf[i], tmp);
        screen_puts(x + o, y, tmp);
        o += strlen(tmp);
        if (i < 5)
            screen_puts(x + (o++), y, ":");
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
  video_ptr=(unsigned char*)(PEEKW(PEEKW(560)+4));
}
