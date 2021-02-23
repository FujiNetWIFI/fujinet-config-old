/**
 * FujiNet Configuration Program
 */

#include <atari.h>
#include <stdbool.h>
#include <string.h>
#include <peekpoke.h>
#include "fuji_typedefs.h"
#include "fuji_sio.h"
#include "mount_and_boot.h"
#include "error.h"
#include "sio.h"
#include "state.h"

Context context;

void main(void)
{
  fuji_sio_read_device_slots(&context.deviceSlots);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_HOST_SLOTS);

  fuji_sio_read_host_slots(&context.hostSlots);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_DEVICE_SLOTS);

  mount_and_boot_mount_all_hosts(&context);
  mount_and_boot_mount_all_devices(&context);

  fuji_sio_set_config_boot(0); // disable ROM disk.

  cold_start(); // reboot

  asm("nop");
}
