/**w
 * #FUJINET Config
 *
 * Mount all device slots, and boot.
 */

#include "mount_and_boot.h"
#include "screen.h"
#include "fuji_sio.h"
#include "fuji_typedefs.h"
#include "error.h"
#include "sio.h"

char text_mounting_host_slot_X[]="MOUNTING HOST SLOT X";
char text_mounting_device_slot_X[]="MOUNTING DEV SLOT X";
char text_booting[]="SUCCESSFUL! BOOTING ";

/**
 * Mount all hosts
 */
void mount_and_boot_mount_all_hosts(Context *context)
{
  unsigned char i;
  
  fuji_sio_read_device_slots(&context->deviceSlots);
  
  for (i=0;i<8;i++)
    {
      if (context->deviceSlots.slot[i].hostSlot != 0xFF)
	{
	  text_mounting_host_slot_X[19]=i+0x31; // update status msg.
	  screen_puts(0,21,text_mounting_host_slot_X);
	  fuji_sio_mount_host(context->deviceSlots.slot[i].hostSlot,&context->hostSlots);
	  if (fuji_sio_error())
	    error_fatal(ERROR_MOUNTING_HOST_SLOT);
	}
    }
}

/**
 * Mount all devices
 */
void mount_and_boot_mount_all_devices(Context *context)
{
  unsigned char i;
  
  for (i=0;i<8;i++)
    {
      if (context->deviceSlots.slot[i].hostSlot != 0xFF)
	{
	  text_mounting_device_slot_X[18]=i+0x31; // update status msg
	  screen_puts(0,21,text_mounting_device_slot_X);
	  fuji_sio_mount_device(i,context->deviceSlots.slot[i].mode);
	  if (fuji_sio_error())
	    error_fatal(ERROR_MOUNTING_DEVICE_SLOT);
	}
    }

  screen_puts(0,21,text_booting);
}

/**
 * Mount all hosts attached to device slots, and boot.
 */
State mount_and_boot(Context *context)
{
  screen_dlist_mount_and_boot();

  fuji_sio_read_device_slots(&context->deviceSlots);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_HOST_SLOTS);
  
  fuji_sio_read_host_slots(&context->hostSlots);
  if (fuji_sio_error())
    error_fatal(ERROR_READING_DEVICE_SLOTS);
  
  screen_puts(0,0,"   MOUNT AND BOOT   ");
  
  mount_and_boot_mount_all_hosts(context);
  mount_and_boot_mount_all_devices(context);

  cold_start(); // reboot
  
  return DISKULATOR_HOSTS; // Never gets here, but we set anyway.
}

