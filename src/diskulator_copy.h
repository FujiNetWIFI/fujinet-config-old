/**
 * #FUJINET CONFIG
 * Diskulator Copy to slot screen
 */

#ifndef DISKULATOR_COPY_H
#define DISKULATOR_COPY_H

#include "state.h"

/**
 * Setup Diskulator Disk Images screen
 */
void diskulator_copy_setup(Context *context);

/**
 * Select Disk Image screen
 */
State diskulator_copy(Context *context);

#endif /* DISKULATOR_COPY_H */
