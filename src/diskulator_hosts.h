/**
 * #FUJINET CONFIG
 * Diskulator Hosts / Device Slots screen
 */

#ifndef DISKULATOR_HOSTS_H
#define DISKULATOR_HOSTS_H

#include "state.h"

/**
 * Display device slots
 */
void diskulator_hosts_display_device_slots(unsigned char y, DeviceSlots *ds);

/**
 * Set wifi State
 */
State diskulator_hosts(Context *context);

#endif /* DISKULATOR_HOSTS_H */
