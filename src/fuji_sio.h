/**
 * #FujiNet SIO calls
 */

#ifndef FUJINET_SIO_H
#define FUJINET_SIO_H



/**
 * Read host slots
 */
void fuji_sio_read_host_slots(void);

/**
 * Write host slots
 */
void fuji_sio_write_host_slots(void);

/**
 * Mount a host slot
 */
void fuji_sio_mount_host(unsigned char c);

/**
 * Read drive tables
 */
void fuji_sio_read_device_slots(void);

/**
 * Write drive tables
 */
void fuji_sio_write_device_slots(void);

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
void fuji_sio_new_disk(unsigned char c, unsigned short ns, unsigned short ss);

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
