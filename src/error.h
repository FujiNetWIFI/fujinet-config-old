/**
 * #FUJINET CONFIG
 *
 * Error Display Routines
 */

#ifndef ERROR_H
#define ERROR_H

typedef enum _errorMessage
  {
   ERROR_READING_ADAPTER_CONFIG,
   ERROR_SCANNING_NETWORKS,
   ERROR_READING_SCAN_RESULT,
   ERROR_SETTING_SSID
  } ErrorMessage;

/**
 * Display a fatal error, then die.
 * 
 * msg = An error message from the enum
 */
void error_fatal(ErrorMessage msg);

#endif /* ERROR_H */
