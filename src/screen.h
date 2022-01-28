/**
 * FujiNet Configuration Program
 *
 * Screen macros for defining character display attribtues for a select set of keys, variables and functions. 
 */

#ifndef SCREEN_H
#define SCREEN_H

extern unsigned char* cursor_ptr;
#define GRAPHICS_0_SCREEN_SIZE (40*25)  // Defines the memory size in bytes
#define DISPLAY_LIST 0x0600             // Memory address to store DISPLAY_LIST.  0x0600 is the first address available for user space memory (1)
#define DISPLAY_MEMORY 0x7400           // Memory address to store DISPLAY_MEMORY.

/**
 * The following defines assign an Atari internal code (2) character to specific labels used for display.  
 **/

#define CH_FOLDER "\x04"               // Set the character folder to # 
#define CH_SERVER "\x06"               // Set the server folder to atari heart.
#define CH_KEY_LABEL_L "\xD9"          // Left arrow on the keyboard 
#define CH_KEY_LABEL_R "\x19"          // Right arrow on the keyboard 

// Inverse of internal character codes
#define CH_INV_MINUS "\x8d"
#define CH_INV_LT "\x9C"
#define CH_INV_GT "\x9E"

#define CH_INV_A "\xA1"
#define CH_INV_B "\xA2"
#define CH_INV_C "\xA3"
#define CH_INV_D "\xA4"
#define CH_INV_E "\xA5"
#define CH_INV_F "\xA6"
#define CH_INV_G "\xA7"
#define CH_INV_H "\xA8"
#define CH_INV_I "\xA9"
#define CH_INV_J "\xAA"
#define CH_INV_K "\xAB"
#define CH_INV_L "\xAC"
#define CH_INV_M "\xAD"
#define CH_INV_N "\xAE"
#define CH_INV_O "\xAF"
#define CH_INV_P "\xB0"
#define CH_INV_Q "\xB1"
#define CH_INV_R "\xB2"
#define CH_INV_S "\xB3"
#define CH_INV_T "\xB4"
#define CH_INV_U "\xB5"
#define CH_INV_V "\xB6"
#define CH_INV_W "\xB7"
#define CH_INV_X "\xB8"
#define CH_INV_Y "\xB9"
#define CH_INV_Z "\xBA"

#define CH_INV_1 "\x91"
#define CH_INV_2 "\x92"
#define CH_INV_3 "\x93"
#define CH_INV_4 "\x94"
#define CH_INV_5 "\x95"
#define CH_INV_6 "\x96"
#define CH_INV_7 "\x97"
#define CH_INV_8 "\x98"
#define CH_INV_9 "\x99"
#define CH_INV_LEFT "\xDE"
#define CH_INV_RIGHT "\xDF"

#define CH_KEY_1TO8 CH_KEY_LABEL_L CH_INV_1 CH_INV_MINUS CH_INV_8 CH_KEY_LABEL_R
#define CH_KEY_ESC CH_KEY_LABEL_L CH_INV_E CH_INV_S CH_INV_C CH_KEY_LABEL_R
#define CH_KEY_TAB CH_KEY_LABEL_L CH_INV_T CH_INV_A CH_INV_B CH_KEY_LABEL_R
#define CH_KEY_DELETE CH_KEY_LABEL_L CH_INV_D CH_INV_E CH_INV_L CH_INV_E CH_INV_T CH_INV_E CH_KEY_LABEL_R
#define CH_KEY_LEFT CH_KEY_LABEL_L CH_INV_LEFT CH_KEY_LABEL_R
#define CH_KEY_RIGHT CH_KEY_LABEL_L CH_INV_RIGHT CH_KEY_LABEL_R
#define CH_KEY_RETURN CH_KEY_LABEL_L CH_INV_R CH_INV_E CH_INV_T CH_INV_U CH_INV_R CH_INV_N CH_KEY_LABEL_R
#define CH_KEY_OPTION CH_KEY_LABEL_L CH_INV_O CH_INV_P CH_INV_T CH_INV_I CH_INV_O CH_INV_N CH_KEY_LABEL_R
#define CH_KEY_C CH_KEY_LABEL_L CH_INV_C CH_KEY_LABEL_R
#define CH_KEY_N CH_KEY_LABEL_L CH_INV_N CH_KEY_LABEL_R
#define CH_KEY_F CH_KEY_LABEL_L CH_INV_F CH_KEY_LABEL_R
#define CH_KEY_LT CH_KEY_LABEL_L CH_INV_LT CH_KEY_LABEL_R
#define CH_KEY_GT CH_KEY_LABEL_L CH_INV_GT CH_KEY_LABEL_R
#define CH_KEY_ESC CH_KEY_LABEL_L CH_INV_E CH_INV_S CH_INV_C CH_KEY_LABEL_R

/**
 * Define key code to detect during keyboard capture
 */
#define KCODE_RETURN  0x9B  // is the ATASCI equivlant of 155 End Of Line (return)
#define KCODE_ESCAPE  0x1B
#define KCODE_BACKSP  0x7E
/**
 * Main init procedure
 */
void screen_init(void);
/**
 * Print ATASCII string to display memory
 */
void screen_puts(unsigned char x,unsigned char y,char *s);

/**
 * Append ATASCII string to display memory
 */
void screen_append(char *s);

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
#define screen_input(x,y,s) _screen_input((x),(y),(s),sizeof(s))

int _screen_input(unsigned char x, unsigned char y, char* s, unsigned char maxlen);

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

/**
 * Patch the dlist for the diskulator select screen.
 */
void screen_dlist_diskulator_select_aux(void);

/**
 * Patch the dlist for the diskulator slot screen.
 */
void screen_dlist_diskulator_slot(void);

/**
 * Patch the dlist for the diskulator copy host slot screen.
 */
void screen_dlist_diskulator_copy_destination_host_slot(void);

/**
 *  sets the cursor at x,y for further operations
 */
void set_cursor(unsigned char x, unsigned char y);

/**
 * Print ATASCII string to display memory
 */
void put_char(char c);
#endif /* SCREEN_H */

/**
 * Reference
 * 1. Mapping the Atari, pg. 91.   Often to referred to as Page Six.
 * 2. Mapping the Atari, pg. 180. Appendix 10 ATASCII And Internal Character Code Values
 **/
