// Submitted by: Dédjorn Frampton - 816039229
// Q2 - Non-volatile Storage Header File

#ifndef SPIFFS_ROUTINES_H        // Prevents multiple inclusion of this header
#define SPIFFS_ROUTINES_H

#include <stdio.h>               
#include <string.h>              
#include "esp_spiffs.h"          
#include "esp_log.h"             
#include "esp_err.h"             

// Constraints
#define TAG "SPIFFS"             
#define SPIFFS_FILE_PATH "/spiffs/data.txt"     // Path to the file stored in SPIFFS

// Function prototypes
bool init_spiff(void);                          // Initialize and mount SPIFFS
bool stop_spiff(void);                          // Unmount SPIFFS and clean up
uint32_t singleread_spiff(void);                // Read a single uint32_t value
bool singlewrite_spiff(uint32_t value);         // Write a single uint32_t value
int burstread_spiff(uint8_t* buffer, int max_bytes);    // Read multiple bytes
bool burstwrite_spiff(uint8_t* buffer, int byte_count); // Write multiple bytes

#endif