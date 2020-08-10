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
char text_error_reading_net_config[] = "ERROR READING NET CONFIG";
char text_error_setting_ssid[] = "ERROR SETTING SSID";
char text_error_reading_wifi_status[] = "ERROR READING WIFI STATUS";
char text_error_wifi_no_ssid_available[] = "NO SSID AVIALABLE";
char text_error_wifi_connection_successful[] = "CONNECTED TO NETWORK";
char text_error_wifi_connect_failed[] = "CONNECT FAILED";
char text_error_wifi_connection_lost[] = "CONNECTION LOST";
char text_error_wifi_disconnected[] = "WIFI DISCONNECTED";
char text_error_reading_host_slots[] = "ERROR READING HOST SLOTS";
char text_error_reading_device_slots[] = "ERROR READING DEVICE SLOTS";
char text_error_mounting_host_slot[] = "ERROR MOUNTING HOST SLOT";
char text_error_mounting_device_slot[] = "ERROR MOUNTING DEVICE SLOT";
char text_error_setting_directory_position[] = "ERROR SETTING DIRECTORY POS";
char text_error_opening_directory[] = "ERROR OPENING DIRECTORY";
char text_error_reading_directory[] = "ERROR READING DIRECTORY";
char text_error_setting_full_path[] = "ERROR SETTING FULL PATH";
char text_error_writing_device_slots[] = "ERROR WRITING DEVICE SLOTS";
char text_error_intentional_stop[] = "INTENTIONAL STOP";
char text_error_creating_new_disk[] = "ERROR CREATING NEW DISK";

/**
 * Display error.
 * 
 * msg = An error message from the enum
 */
void error(ErrorMessage msg)
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
    case ERROR_READING_NET_CONFIG:
      text = text_error_reading_net_config;
      break;
    case ERROR_SETTING_SSID:
      text = text_error_setting_ssid;
      break;
    case ERROR_READING_WIFI_STATUS:
      text = text_error_reading_wifi_status;
      break;
    case ERROR_WIFI_NO_SSID_AVAILABLE:
      text = text_error_wifi_no_ssid_available;
      break;
    case ERROR_WIFI_CONNECTION_SUCCESSFUL:
      text = text_error_wifi_connection_successful;
      break;
    case ERROR_WIFI_CONNECT_FAILED:
      text = text_error_wifi_connect_failed;
      break;
    case ERROR_WIFI_CONNECTION_LOST:
      text = text_error_wifi_connection_lost;
      break;
    case ERROR_WIFI_DISCONNECTED:
      text = text_error_wifi_disconnected;
      break;
    case ERROR_READING_HOST_SLOTS:
      text = text_error_reading_host_slots;
      break;
    case ERROR_READING_DEVICE_SLOTS:
      text = text_error_reading_device_slots;
      break;
    case ERROR_MOUNTING_HOST_SLOT:
      text = text_error_mounting_host_slot;
      break;
    case ERROR_MOUNTING_DEVICE_SLOT:
      text = text_error_mounting_device_slot;
      break;
    case ERROR_SETTING_DIRECTORY_POSITION:
      text = text_error_setting_directory_position;
      break;
    case ERROR_OPENING_DIRECTORY:
      text = text_error_opening_directory;
      break;
    case ERROR_READING_DIRECTORY:
      text = text_error_reading_directory;
      break;
    case ERROR_SETTING_FULL_PATH:
      text = text_error_setting_full_path;
      break;
    case ERROR_WRITING_DEVICE_SLOTS:
      text = text_error_writing_device_slots;
      break;
    case ERROR_CREATING_NEW_DISK:
      text = text_error_creating_new_disk;
      break;
    case ERROR_INTENTIONAL_STOP:
      text = text_error_intentional_stop;
      break;
    }  
  screen_puts(0,21,text);
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
