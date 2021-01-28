/**
 * #FUJINET CONFIG
 * Diskulator Select Disk Image screen
 */

#ifndef DISKULATOR_SELECT_H
#define DISKULATOR_SELECT_H

#include "state.h"

/**
 * Setup Diskulator Disk Images screen
 */
void diskulator_select_setup(Context *context);

/**
 * Select Disk Image screen
 */
State diskulator_select(Context *context);

/**
 * Display directory path (and "SD" or TNFS server name)
 */
void diskulator_select_display_directory_path(Context* context);

#endif /* DISKULATOR_SELECT_H */
