/**
 * #FUJINET CONFIG
 *
 * Error Display Routines
 */

#ifndef ERROR_H
#define ERROR_H

typedef enum _errorMessage
  {
   ERROR_READING_CONFIG,
   
  } ErrorMessage;

/**
 * Display a fatal error, then die.
 * 
 * msg = An error message from the enum
 */
void error_fatal(ErrorMessage msg);

#endif /* ERROR_H */
