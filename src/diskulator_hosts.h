/**
 * #FUJINET CONFIG
 * Diskulator Hosts / Device Slots screen
 */

#ifndef DISKULATOR_HOSTS_H
#define DISKULATOR_HOSTS_H

#define ORIGIN_HOST_SLOTS 2
#define ORIGIN_DEVICE_SLOTS 13

#include "state.h"

/**
 * Display device slots
 */
void diskulator_hosts_display_device_slots(unsigned char y, DeviceSlots *ds);

/**
 * Eject image from device slot
 */
void diskulator_hosts_eject_device_slot(unsigned char i, unsigned char pos, DeviceSlots* ds);

/**
 * Set device slot to read or write
 */
void diskulator_hosts_set_device_slot_mode(unsigned char i, unsigned char mode, DeviceSlots* ds);

/**
 * Hosts/Devices screen
 */
State diskulator_hosts(Context *context);

#endif /* DISKULATOR_HOSTS_H */
