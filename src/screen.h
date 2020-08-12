/**
 * FujiNet Configuration Program
 *
 * Screen macros
 */

#ifndef SCREEN_H
#define SCREEN_H

#define SetChar(x,y,a) video_ptr[(x)+(y)*40]=(a);
#define GetChar(x,y) video_ptr[(x)+(y)*40]
#define GRAPHICS_0_SCREEN_SIZE (40*25)
#define DISPLAY_LIST 0x0600
#define DISPLAY_MEMORY 0x7400
#define FONT_MEMORY 0x7800

/**
 * Print ATASCII string to display memory
 */
void screen_puts(unsigned char x,unsigned char y,char *s);

/**
 * Clear a line
 */
void screen_clear_line(unsigned char y);

/**
 * Clear screen
 */
void screen_clear(void);

/**
 * Input a string at x,y
 */
int screen_input(unsigned char x, unsigned char y, char* s);

/**
 * Print IP address at position
 */
void screen_print_ip(unsigned char x, unsigned char y, unsigned char *buf);

/**
 * Print MAC address at position
 */
void screen_print_mac(unsigned char x, unsigned char y, unsigned char *buf);

/**
 * Patch the dlist for the hosts screen
 */
void screen_dlist_diskulator_hosts(void);

/**
 * Patch the dlist for the info screen
 */
void screen_dlist_diskulator_info(void);

/**
 * Patch the dlist for wifi screens
 */
void screen_dlist_wifi(void);

/**
 * Patch the dlist for mount and boot screen.
 */
void screen_dlist_mount_and_boot(void);

/**
 * Patch the dlist for the diskulator select screen.
 */
void screen_dlist_diskulator_select(void);

#endif /* SCREEN_H */
