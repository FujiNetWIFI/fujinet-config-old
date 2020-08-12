/**
 * #FUJINET Config
 *
 * Mount all device slots, and boot.
 */

#ifndef MOUNT_AND_BOOT_H
#define MOUNT_AND_BOOT_H

#include "state.h"

/**
 * Mount all hosts attached to device slots, and boot.
 */
State mount_and_boot(Context *context);

#endif /* MOUNT_AND_BOOT_H */
