/**w
 * #FUJINET Config
 *
 * Mount all device slots, and boot.
 */

#include "mount_and_boot.h"
#include "fuji_sio.h"
#include "fuji_typedefs.h"
#include "error.h"
#include "sio.h"
#include "die.h"

/**
 * Mount all hosts
 */
void mount_and_boot_mount_all_hosts(Context *context)
{
  unsigned char i;
  unsigned char retry=5;
  
  fuji_sio_read_device_slots(&context->deviceSlots);
  
  for (i=0;i<8;i++)
    {
      if (context->deviceSlots.slot[i].hostSlot != 0xFF)
	{
	  while (retry>0)
	    {
	      fuji_sio_mount_host(context->deviceSlots.slot[i].hostSlot,&context->hostSlots);
	      if (fuji_sio_error())
		retry--;
	      else
		break;
	    }

	  if (fuji_sio_error())
	    {
	      error(ERROR_MOUNTING_HOST_SLOT);
	      wait_a_moment();
	      context->state=DISKULATOR_HOSTS;
	      return;
	    }
	}
    }
}

/**
 * Mount all devices
 */
void mount_and_boot_mount_all_devices(Context *context)
{
  unsigned char i;
  unsigned char retry=5;
  
  for (i=0;i<8;i++)
    {
		if (context->deviceSlots.slot[i].hostSlot != 0xFF)
		{
		  while (retry>0)
			{
			  fuji_sio_mount_device(i,context->deviceSlots.slot[i].mode);

			  if (fuji_sio_error())
			retry--;
			  else
			break;
			}

		  if (fuji_sio_error())
			{
			  error(ERROR_MOUNTING_DEVICE_SLOT);
			  wait_a_moment();
			  context->state=DISKULATOR_HOSTS;
			  return;
			}
		}
    }
}
