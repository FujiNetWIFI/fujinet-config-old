/**
 * #FUJINET Config
 *
 * Mount all device slots, and boot.
 */

#ifndef MOUNT_AND_BOOT_H
#define MOUNT_AND_BOOT_H

#include "state.h"

void mount_and_boot_mount_all_hosts(Context *context);

void mount_and_boot_mount_all_devices(Context *context);

#endif /* MOUNT_AND_BOOT_H */
