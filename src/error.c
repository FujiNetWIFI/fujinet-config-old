/**
 * #FUJINET CONFIG
 *
 * Error Display Routines
 */

#include <stddef.h> // for NULL
#include "die.h"
#include "error.h"
#include "screen.h"

char text_error_reading_adapter_config[] = "ERROR READING ADAPTER CONFIG";
char text_error_scanning_networks[] = "ERROR SCANNING NETWORKS";
char text_error_reading_scan_result[] = "ERROR READING SCAN RESULT";
char text_error_setting_ssid[] = "ERROR SETTING SSID";

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
    case ERROR_READING_ADAPTER_CONFIG:
      text = text_error_reading_adapter_config;
      break;
    case ERROR_SCANNING_NETWORKS:
      text = text_error_scanning_networks;
      break;
    case ERROR_READING_SCAN_RESULT:
      text = text_error_reading_scan_result;
      break;
    case ERROR_SETTING_SSID:
      text = text_error_setting_ssid;
      break;
    }
  
  screen_puts(0,21,text);
  die();
}
