/**
 * #FujiNet SIO calls
 */

#include <atari.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "sio.h"
#include "fuji_sio.h"

void set_sio_defaults(void)
{
    OS.dcb.ddevic = 0x70;
    OS.dcb.dunit = 1;
    OS.dcb.dtimlo = 0x0F; // 15 second timeout
}

/**
 * Return number of networks
 */
unsigned char fuji_sio_do_scan(unsigned char *num_networks)
{
    set_sio_defaults();
    OS.dcb.dcomnd = 0xFD; // do scan
    OS.dcb.dstats = 0x40; // Peripheral->Computer
    OS.dcb.dbuf = num_networks;
    OS.dcb.dbyt = 4;      // 4 byte response
    OS.dcb.daux = 0;
    siov();

    return OS.dcb.dstats;
}

/**
 * Return Network entry from last scan
 */
unsigned char fuji_sio_scan_result(unsigned char n, SSIDInfo* ssidInfo)
{
    set_sio_defaults();
    OS.dcb.dcomnd = 0xFC; // Return scan result
    OS.dcb.dstats = 0x40; // Peripheral->Computer
    OS.dcb.dbuf = &ssidInfo->rawData;
    OS.dcb.dbyt = sizeof(ssidInfo->rawData);
    OS.dcb.daux1 = n; // get entry #n
    siov();

    return OS.dcb.dstats;
}

/**
 * Write desired SSID and password to SIO
 */
unsigned char fuji_sio_set_ssid(bool save, NetConfig* netConfig)
{
    set_sio_defaults();
    OS.dcb.dcomnd = 0xFB; // Set SSID
    OS.dcb.dstats = 0x80; // Computer->Peripheral
    OS.dcb.dbuf = &netConfig->rawData;
    OS.dcb.dbyt = sizeof(netConfig->rawData);
    OS.dcb.daux = save ? 1 : 0;
    siov();

    return OS.dcb.dstats;
}

/**
 * Get WiFi Network Status
 */
unsigned char fuji_sio_get_wifi_status(unsigned char* wifiStatus)
{
    set_sio_defaults();
    OS.dcb.dcomnd = 0xFA; // Return wifi status
    OS.dcb.dstats = 0x40; // Peripheral->Computer
    OS.dcb.dbuf = &wifiStatus;
    OS.dcb.dbyt = 1;
    OS.dcb.daux1 = 0;
    siov();

    return OS.dcb.dstats;
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
 * Write host slots
 */
void fuji_sio_write_host_slots(HostSlots* hostSlots)
{
    set_sio_defaults();
    OS.dcb.dcomnd = 0xF3;
    OS.dcb.dstats = 0x80;
    OS.dcb.dbuf = &hostSlots->rawData;
    OS.dcb.dbyt = 256;
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
 * Write drive tables
 */
void fuji_sio_write_device_slots(DeviceSlots* deviceSlots)
{
    set_sio_defaults();
    OS.dcb.dcomnd = 0xF1;
    OS.dcb.dstats = 0x80;
    OS.dcb.dbuf = &deviceSlots->rawData;
    OS.dcb.dbyt = sizeof(deviceSlots->rawData);
    OS.dcb.daux = 0;
    siov();
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
    OS.dcb.daux1 = c;
    OS.dcb.daux2 = o;
    siov();
}

/**
 * Mount device slot
 */
void fuji_sio_umount_device(unsigned char c)
{
    set_sio_defaults();
    OS.dcb.dcomnd = 0xE9;
    OS.dcb.dstats = 0x00;
    OS.dcb.dbuf = NULL;
    OS.dcb.dbyt = 0;
    OS.dcb.daux = c;
    siov();
}

/**
 * Create New Disk
 */
void fuji_sio_new_disk(unsigned char c, unsigned short ns, unsigned short ss, DeviceSlots* deviceSlots)
{
  NewDisk newDisk;
  newDisk.numSectors = ns;
  newDisk.sectorSize = ss;
  newDisk.hostSlot = deviceSlots->slot[c].hostSlot;
  newDisk.deviceSlot = c;
  strcpy(newDisk.filename, deviceSlots->slot[c].file);
  deviceSlots->slot[c].mode = 0x02;
  
  set_sio_defaults();
  OS.dcb.dcomnd = 0xE7; // TNFS Create Disk
  OS.dcb.dstats = 0x80;
  OS.dcb.dbuf = &newDisk.rawData;
  OS.dcb.dtimlo = 0xFE;
  OS.dcb.dbyt = sizeof(newDisk.rawData);
  OS.dcb.daux = 0;
  siov();
}

/**
 * Open Directory
 */
void fuji_sio_open_directory(unsigned char hs, char *p)
{
    // Open Dir
    set_sio_defaults();
    OS.dcb.dcomnd = 0xF7;
    OS.dcb.dstats = 0x80;
    OS.dcb.dbuf = p;
    OS.dcb.dbyt = 256;
    OS.dcb.daux = hs;
    siov();
}

/**
 * Read next dir entry
 */
void fuji_sio_read_directory(char *e, unsigned short len)
{
    memset(e, 0, len);
    e[0] = 0x7f;
    OS.dcb.dcomnd = 0xF6;
    OS.dcb.dstats = 0x40;
    OS.dcb.dbuf = e;
    OS.dcb.dbyt = len;
    OS.dcb.daux = len;
    siov();
}

/**
 * Get current directory cursor
 */
DirectoryPosition fuji_sio_get_directory_position(void)
{
  DirectoryPosition pos=0;
  
  OS.dcb.dcomnd = 0xE5;
  OS.dcb.dstats = 0x40;
  OS.dcb.dbuf = &pos;
  OS.dcb.dbyt = sizeof(pos);
  OS.dcb.daux = 0;
  siov();
  return pos;
}

/**
 * Set current directory cursor
 */
void fuji_sio_set_directory_position(DirectoryPosition pos)
{
  OS.dcb.dcomnd = 0xE4;
  OS.dcb.dstats = 0x00;
  OS.dcb.dbuf = NULL;
  OS.dcb.dbyt = 0;
  OS.dcb.daux = pos;
  siov();
}

/**
 * Close directory
 */
void fuji_sio_close_directory(unsigned char hs)
{
    // Close dir read
    OS.dcb.dcomnd = 0xF5;
    OS.dcb.dstats = 0x00;
    OS.dcb.dbuf = NULL;
    OS.dcb.dtimlo = 0x0f;
    OS.dcb.dbyt = 0;
    OS.dcb.daux = hs;
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
 * Read #FujiNet configuration
 */
void fuji_sio_read_config(NetConfig* netConfig)
{
  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dcomnd = 0xFE; //get_ssid
  OS.dcb.dstats = 0x40;
  OS.dcb.dbuf = netConfig->rawData;
  OS.dcb.dtimlo = 0x0F; // 15 second timeout
  OS.dcb.dbyt = sizeof(netConfig->rawData);
  OS.dcb.daux = 0;
  siov();
}
