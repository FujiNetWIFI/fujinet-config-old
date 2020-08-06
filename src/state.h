/**
 * #FUJINET Configuration
 *
 * State Machine and context
 */

#ifndef STATE_H
#define STATE_H

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
  char directory[256];               // Current directory
  char filename[256];                // Current filename
  unsigned char host_slot;           // Current Host slot (0-7)
  unsigned char device_slot;         // Current Device slot (0-7)
  unsigned short dir_pos;            // current directory position
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
