/**
 * #FUJINET CONFIG
 *
 * Error Display Routines
 */

#include <stddef.h> // for NULL
#include "die.h"
#include "error.h"
#include "screen.h"

char text_error_reading_config[] = "ERROR READING ADAPTER CONFIG";
char text_error_scanning_networks[] = "ERROR SCANNING NETWORKS";

/**
 * Display a fatal error, then die.
 * 
 * msg = An error message from the enum
 */
void error_fatal(ErrorMessage msg)
{
  char* text = NULL;
  
  switch (msg)
    {
    case ERROR_READING_CONFIG:
      text = text_error_reading_config;
      break;
    case ERROR_SCANNING_NETWORKS:
      text = text_error_scanning_networks;
      break;
    }
  
  screen_puts(0,21,text);
  die();
}
