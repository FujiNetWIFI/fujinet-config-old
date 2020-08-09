/**
 * #FUJINET Configuration
 *
 * State Machine and context
 */

#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include "fuji_typedefs.h"

typedef enum _state
  {
   SET_WIFI,
   CONNECT_WIFI,
   DISKULATOR_HOSTS,
   DISKULATOR_SELECT,
   DISKULATOR_SLOT,
   DISKULATOR_INFO,
   ADAPTER_CONFIG,
   MOUNT_AND_BOOT
  } State;

typedef struct _context
{
  State state;                       // Current program state (state.h)
  char directory[128];               // Current directory
  char filename[128];                // Current filename
  char full_path[256];               // Full path.
  unsigned char host_slot;           // Current Host slot (0-7)
  unsigned char device_slot;         // Current Device slot (0-7)
  unsigned char mode;                // mode for device slot (1 or 2)
  unsigned short dir_pos;            // current directory position
  unsigned short dir_page;           // directory page
  bool dir_eof;                      // End of current directory?
  unsigned char entries_displayed;   // # of entries displayed on current page.
  DeviceSlots deviceSlots;           // Current device slots
  HostSlots hostSlots;               // Current host slots.
} Context;

/**
 * Set up initial context
 */
void context_setup(Context* context);

/**
 * State machine
 */
void state(Context *context);

#endif /* STATE_H */
