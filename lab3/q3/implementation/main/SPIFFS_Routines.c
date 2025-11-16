// Submitted by: Dédjorn Frampton - 816039229
// Q2 - Non-volatile Storage Routines

#include <stdio.h>
#include <string.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "SPIFFS_Routines.h"    // Include header for prototypes

// Constraints
#define TAG "SPIFFS"
#define SPIFFS_FILE_PATH "/spiffs/data.txt"     // Path to the file stored in SPIFFS


// Global state variable
static bool spiffs_active = false;      // Indicates if SPIFFS is mounted and active
static FILE* current_file = NULL;       // File handle for the current open file (if any)


// SPIFFS configuration structure
static esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",             // Mount point
    .partition_label = NULL,            // Use default SPIFFS partition
    .max_files = 5,                     // Maximum files open at once
    .format_if_mount_failed = true      // Format partition if mount fails
};



// Initialize and mount SPIFFS
bool init_spiff(void) {
    esp_err_t ret = esp_vfs_spiffs_register(&conf);     // Attempt to mount SPIFFS
    
    if (ret != ESP_OK) {                                // Error handling
        if (ret == ESP_ERR_NO_MEM) {
            ESP_LOGE(TAG, "Unable to allocate memory for SPIFFS");
        } else if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "SPIFFS is already mounted or in invalid state");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS: %s", esp_err_to_name(ret));
        }
        return false;
    }
    
    // Get SPIFFS partition size and usage
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS info: %s", esp_err_to_name(ret));
        esp_vfs_spiffs_unregister(conf.partition_label);        // Clean up if info fails
        return false;
    }
    
    spiffs_active = true;           // Mark SPIFFS as active
    ESP_LOGI(TAG, "SPIFFS initialized successfully");
    ESP_LOGI(TAG, "Partition size: %d bytes, Used: %d bytes", total, used);
    return true;
}

// Unmount SPIFFS and clean up
bool stop_spiff(void) {
    if (!spiffs_active) {           // Check if SPIFFS is active
        ESP_LOGW(TAG, "SPIFFS not active");
        return false;
    }
    
    if (current_file != NULL) {     // Close any open file
        fclose(current_file);
        current_file = NULL;
    }
    
    esp_err_t ret = esp_vfs_spiffs_unregister(conf.partition_label);    // Unmount SPIFFS
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unmount SPIFFS: %s", esp_err_to_name(ret));
        return false;
    }
    
    spiffs_active = false;          // Mark SPIFFS as inactive
    ESP_LOGI(TAG, "SPIFFS stopped successfully");
    return true;
}

// Reads a single uint32_t value from SPIFFS
uint32_t singleread_spiff(void) {
    if (!spiffs_active) {
        ESP_LOGE(TAG, "SPIFFS not active");
        return 0;
    }
    
    FILE* f = fopen(SPIFFS_FILE_PATH, "r");     // Open file for reading
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return 0;
    }
    
    uint32_t value = 0;
    size_t bytes_read = fread(&value, sizeof(uint32_t), 1, f);      // Read one word
    fclose(f);
    
    if (bytes_read != 1) {
        ESP_LOGW(TAG, "Failed to read complete value");
        return 0;
    }
    
    ESP_LOGI(TAG, "Single read: %u", value);
    return value;
}

// Writes a single uint32_t value to SPIFFS
bool singlewrite_spiff(uint32_t value) {
    if (!spiffs_active) {
        ESP_LOGE(TAG, "SPIFFS not active");
        return false;
    }
    
    FILE* f = fopen(SPIFFS_FILE_PATH, "a");     // Open file for appending
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return false;
    }
    
    size_t bytes_written = fwrite(&value, sizeof(uint32_t), 1, f);  // Write one word
    fclose(f);
    
    if (bytes_written != 1) {
        ESP_LOGE(TAG, "Failed to write complete value");
        return false;
    }
    
    ESP_LOGI(TAG, "Single write: %u", value);
    return true;
}

// Burst read: read multiple bytes from SPIFFS
int burstread_spiff(uint8_t* buffer, int max_bytes) {
    if (!spiffs_active) {
        ESP_LOGE(TAG, "SPIFFS not active");
        return 0;
    }
    
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Invalid buffer pointer");
        return 0;
    }
    
    FILE* f = fopen(SPIFFS_FILE_PATH, "r");   // Open file for reading
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return 0;
    }
    
    // If max_bytes == 0, read entire file
    int bytes_to_read = max_bytes;
    if (max_bytes == 0) {
        fseek(f, 0, SEEK_END);
        bytes_to_read = ftell(f);   // Get file size
        fseek(f, 0, SEEK_SET);
    }
    
    int bytes_read = fread(buffer, 1, bytes_to_read, f); // Read into buffer
    fclose(f);
    
    ESP_LOGI(TAG, "Burst read: %d bytes", bytes_read);
    return bytes_read;
}

// Burst write: write multiple bytes to SPIFFS
bool burstwrite_spiff(uint8_t* buffer, int byte_count) {
    if (!spiffs_active) {
        ESP_LOGE(TAG, "SPIFFS not active");
        return false;
    }
    
    if (buffer == NULL || byte_count <= 0) {
        ESP_LOGE(TAG, "Invalid buffer or byte count");
        return false;
    }
    
    FILE* f = fopen(SPIFFS_FILE_PATH, "a");   // Open file for appending
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return false;
    }
    
    int bytes_written = fwrite(buffer, 1, byte_count, f); // Write buffer
    fclose(f);
    
    if (bytes_written != byte_count) {
        ESP_LOGE(TAG, "Failed to write all bytes: wrote %d of %d", bytes_written, byte_count);
        return false;
    }
    
    ESP_LOGI(TAG, "Burst write: %d bytes", bytes_written);
    return true;
}
