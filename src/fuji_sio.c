/**
 * #FujiNet SIO calls
 */

#include <atari.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "sio.h"
#include "fuji_sio.h"

/**
 * Did an error result?
 */
bool fuji_sio_error(void)
{
  return (OS.dcb.dstats>128);
}

void set_sio_defaults(void)
{
  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dtimlo = 0x0F; // 15 second timeout
}

/**
 * Read drive tables
 */
void fuji_sio_read_device_slots(DeviceSlots* deviceSlots)
{
    // Read Drive Tables
    set_sio_defaults();
    OS.dcb.dcomnd = 0xF2;
    OS.dcb.dstats = 0x40;
    OS.dcb.dbuf = &deviceSlots->rawData;
    OS.dcb.dbyt = sizeof(deviceSlots->rawData);
    OS.dcb.daux = 0;
    siov();
}

/**
 * Read host slots
 */
void fuji_sio_read_host_slots(HostSlots* hostSlots)
{
    // Query for host slots
    set_sio_defaults();
    OS.dcb.dcomnd = 0xF4; // Get host slots
    OS.dcb.dstats = 0x40;
    OS.dcb.dbuf = &hostSlots->rawData;
    OS.dcb.dbyt = sizeof(hostSlots->rawData);
    OS.dcb.daux = 0;
    siov();
}

/**
 * Mount a host slot
 */
void fuji_sio_mount_host(unsigned char c, HostSlots* hostSlots)
{
    if (hostSlots->host[c][0] != 0x00)
    {
        set_sio_defaults();
        OS.dcb.dcomnd = 0xF9;
        OS.dcb.dstats = 0x00;
        OS.dcb.dbuf = NULL;
        OS.dcb.dbyt = 0;
        OS.dcb.daux = c;
        siov();
    }
}

/**
 * Mount device slot
 */
void fuji_sio_mount_device(unsigned char c, unsigned char o)
{
    set_sio_defaults();
    OS.dcb.dcomnd = 0xF8;
    OS.dcb.dstats = 0x00;
    OS.dcb.dbuf = NULL;
    OS.dcb.dbyt = 0;
    OS.dcb.dtimlo = 0xFE; // Due to ATX support.
    OS.dcb.daux1 = c;
    OS.dcb.daux2 = o;
    siov();
}

/**
 * Mount all Hosts
 */
bool fuji_sio_mount_all_hosts(DeviceSlots* deviceSlots, HostSlots* hostSlots)
{
  unsigned char ds;
  
  for (ds = 0; ds < 8; ds++)
    {
      if (deviceSlots->slot[ds].hostSlot != 0xFF)
	fuji_sio_mount_host(deviceSlots->slot[ds].hostSlot, hostSlots);
      
      if (OS.dcb.dstats != 0x01)
	return false; // Mount error
    }
}

/**
 * Mount all devices
 */
bool fuji_sio_mount_all_devices(DeviceSlots* deviceSlots)
{
    unsigned char ds;

    for (ds = 0; ds < 8; ds++)
    {
        if (deviceSlots->slot[ds].hostSlot != 0xFF)
            fuji_sio_mount_device(ds, deviceSlots->slot[ds].mode);

        if (OS.dcb.dstats != 0x01)
	  return false; // Mount error
    }
}

/**
 * Set config boot flag
 */
void fuji_sio_set_config_boot(unsigned char b)
{
  OS.dcb.dcomnd = 0xD9;
  OS.dcb.dstats = 0x00;
  OS.dcb.dbuf = NULL;
  OS.dcb.dtimlo = 0x0f;
  OS.dcb.dbyt = 0;
  OS.dcb.daux1 = b;
  siov();
}
