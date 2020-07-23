/**
 * #FujiNet SIO calls
 */

#ifndef FUJINET_SIO_H
#define FUJINET_SIO_H

/**
 * Return number of networks
 */
unsigned char fuji_sio_do_scan(unsigned char *num_networks);

/**
 * Return Network entry from last scan
 */
unsigned char fuji_sio_scan_result(unsigned char n, SSIDInfo* ssidInfo);

/**
 * Write desired SSID and password to SIO
 */
unsigned char fuji_sio_set_ssid(bool save, NetConfig* netConfig);

/**
 * Get WiFi Network Status
 */
unsigned char fuji_sio_get_wifi_status(unsigned char* wifiStatus);

/**
 * Read host slots
 */
void fuji_sio_read_host_slots(HostSlots* hostSlots);

/**
 * Write host slots
 */
void fuji_sio_write_host_slots(HostSlots* hostSlots);

/**
 * Mount a host slot
 */
void fuji_sio_mount_host(unsigned char c, HostSlots* hostSlots);

/**
 * Read drive tables
 */
void fuji_sio_read_device_slots(DeviceSlots* deviceSlots);

/**
 * Write drive tables
 */
void fuji_sio_write_device_slots(DeviceSlots* deviceSlots);

/**
 * Mount device slot
 */
void fuji_sio_mount_device(unsigned char c, unsigned char o);

/**
 * Mount device slot
 */
void fuji_sio_umount_device(unsigned char c);

/**
 * Create New Disk
 */
void fuji_sio_new_disk(unsigned char c, unsigned short ns, unsigned short ss, DeviceSlots* deviceSlots);

/**
 * Open Directory
 */
void fuji_sio_open_directory(unsigned char hs, char *p);

/**
 * Read next dir entry
 */
void fuji_sio_read_directory(unsigned char hs, char *e, unsigned short len);

/**
 * Close directory
 */
void fuji_sio_close_directory(unsigned char hs);

#endif /* FUJINET_SIO_H */
