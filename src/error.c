/**
 * #FUJINET CONFIG
 *
 * Error Display Routines
 */

#include <stddef.h> // for NULL
#include "die.h"
#include "error.h"
#include "screen.h"

/* Note: These message strings describe the error codes
   in the `ErrorMessage` enum, found in "errors.h".
   (This list must be kept synchronized with that list.) */
char * error_texts[]= {
"ERROR READING ADAPTER CONFIG",
"ERROR SCANNING NETWORKS",
"ERROR READING SCAN RESULT",
"ERROR READING NET CONFIG",
"ERROR SETTING SSID",
"ERROR READING WIFI STATUS",
"NO SSID AVIALABLE",
"CONNECTED TO NETWORK",
"CONNECT FAILED",
"CONNECTION LOST",
"WIFI DISCONNECTED",
"ERROR READING HOST SLOTS",
"ERROR READING DEVICE SLOTS",
"ERROR MOUNTING HOST SLOT",
"ERROR MOUNTING DEVICE SLOT",
"ERROR SETTING DIRECTORY POS",
"ERROR OPENING DIRECTORY",
"ERROR READING DIRECTORY",
"ERROR SETTING FULL PATH",
"ERROR WRITING DEVICE SLOTS",
"ERROR CREATING NEW DISK",
"INTENTIONAL STOP",
"SD CARD EMPTY"
};

/**
 * Display error.
 * 
 * msg = An error message from the enum
 */
void error(ErrorMessage msg)
{
  char* text = NULL;
  screen_clear_line(21);  
  screen_puts(0,21,error_texts[msg]);
}

/**
 * Display a fatal error, then die.
 * 
 * msg = An error message from the enum
 */
void error_fatal(ErrorMessage msg)
{
  error(msg);
  die();
}
