/**
 * #FUJINET Config
 *
 * Common input routines
 */

#ifndef INPUT_H
#define INPUT_H

/**
 * Handle common nav keys
 * k is the input ATASCII key
 * max is the maximum row
 * i is the current index
 */
void input_handle_nav_keys(char k, unsigned char max, unsigned char *i);

/**
 * Get an input from keyboard/joystick
 * Returns an 'atascii key' regardless of input
 */
unsigned char input_handle_key(void);

#endif /* INPUT_H */
