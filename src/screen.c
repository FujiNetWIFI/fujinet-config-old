/**
 * FujiNet Configuration Program
 *
 * Screen functions
 */

#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "screen.h"

unsigned char* video_ptr;
unsigned char* dlist_ptr;
unsigned short screen_memory;
unsigned char* font_ptr;

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
void screen_puts(unsigned char x,unsigned char y,char *s)
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
int screen_input(unsigned char x, unsigned char y, char* s)
{
  unsigned char c,k,o;
  unsigned char outc[2]={0,0};
  unsigned char q;
  
  o=0;
  c=x;

  for (q=0;q<strlen(s);q++)
    if (s[q]==0x00)
      break;
    else
      {
	c++;
	o++;
      }
  
  SetChar(c+1,y,0x80); // turn on cursor

  k=0;
  
  while (k!=155)
    {
      k=cgetc();

      if ( k== 0x1B ) // ESC key to cancel
        return -1;

      if ((k==0x7E) && (c>x)) // backspace
	{
	  SetChar(c+1,y,0);
	  s[--o]=0;
	  c--;
	  SetChar(c+1,y,0x80);
	}
      else if (k==0x9b) // return (EOL)
	{
	  SetChar(c+1,y,GetChar(c+1,y)&0x7F);
	  // exit while.
	}
      else if (((k>0x20) && (k<0x7b) || (k==0x20))) // printable ascii
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
 * Print MAC address at position
 */
void screen_print_mac(unsigned char x, unsigned char y, unsigned char *buf)
{
    unsigned char i = 0;
    unsigned char o = 0;
    unsigned char tmp[3];

    for (i = 0; i < 6; i++)
    {
        itoa(buf[i], tmp, 16);
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
  dlist_ptr[0x0F] = dlist_ptr[0x10] = 6;
  dlist_ptr[0x0A] = dlist_ptr[0x0B] = 2;
  dlist_ptr[0x1B] = dlist_ptr[0x1C] = 6;
}

/**
 * Patch the dlist for the info screen
 */
void screen_dlist_diskulator_info(void)
{
  dlist_ptr[0x0A] = 7;
  dlist_ptr[0x0B] = 6;
  dlist_ptr[0x0F] = dlist_ptr[0x10] = 2;
}

/**
 * Patch the dlist for wifi screens
 */
void screen_dlist_wifi(void)
{
  dlist_ptr[0x0a] = dlist_ptr[0x0b] = 2;
  dlist_ptr[0x1b] = dlist_ptr[0x1c] = 6;
}

/**
 * Patch the dlist for mount and boot screen.
 */
void screen_dlist_mount_and_boot(void)
{
  dlist_ptr[0x0a] = dlist_ptr[0x0b] = 2;
  dlist_ptr[0x1b] = dlist_ptr[0x1c] = 6;
}

/**
 * Patch the dlist for the diskulator select screen.
 */
void screen_dlist_diskulator_select(void)
{
  dlist_ptr[0x0f] = dlist_ptr[0x10] = 2;
  dlist_ptr[0x1B] = dlist_ptr[0x1C] = 6;
}

/**
 * Patch the dlist for the diskulator select screen.
 */
void screen_dlist_diskulator_select_aux(void)
{
  dlist_ptr[0x0f] = dlist_ptr[0x10] = 2;
  dlist_ptr[0x1B] = dlist_ptr[0x1C] = 2;
}

/**
 * Patch the dlist for the diskulator slot screen.
 */
void screen_dlist_diskulator_slot(void)
{
  dlist_ptr[0x0f] = dlist_ptr[0x10] = 2;
  dlist_ptr[0x1B] = dlist_ptr[0x1C] = 6;
}
