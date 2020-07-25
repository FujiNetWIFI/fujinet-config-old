/**
 * Fujinet Configurator
 *
 * Diskulator 
 */

#ifndef DISKULATOR_H
#define DISKULATOR_H

/**
 * Process key for hosts or devices
 */
char diskulator_process_key(void);

/**
 * Cursor up for either hosts or devices
 */
void diskulator_cursor_up(unsigned char* i);

/**
 * Cursor down for either hosts or devices
 */
void diskulator_cursor_down(unsigned char* i, unsigned char max);

/**
 * Run the Diskulator
 */
void diskulator_run(void);

#endif /* DISKULATOR_H */
