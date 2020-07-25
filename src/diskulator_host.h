/**
 * DISKULATOR HOST/DEVICESLOTS
 */

#ifndef DISKULATOR_HOST_H
#define DISKULATOR_HOST_H

#include <stdbool.h>

typedef enum _host_mode
  {
   HOSTS,
   DEVICES,
   CONFIG,
   SELECTED
  } HostMode;

/**
 * Diskulator hosts/deviceslots screen.
 */
bool diskulator_host(unsigned char* selected_host);

#endif
