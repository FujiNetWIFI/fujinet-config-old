/**
 * FujiNet Configurator
 *
 * Function to die, wait for keypress and then do coldstart
 */

#ifndef DIE_H
#define DIE_H

/**
 * Do cold start
 */
void cold_boot(void);

/**
 * Stop, wait for keypress, then coldstart
 */
void die(void);

#endif /* DIE_H */
