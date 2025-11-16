// Submitted by: Dédjorn Frampton - 816039229
// Q2 - Unit Test for Non-volatile Storage Routines

#include "SPIFFS_Routines.h"

#define TAG "SPIFFS_TEST"

void app_main(void) {
    ESP_LOGI(TAG, "Starting SPIFFS unit test...");

    if (!init_spiff()) {
        ESP_LOGE(TAG, "SPIFFS initialization failed");
        return;
    }

    // Single write/read test
    singlewrite_spiff(12345);
    uint32_t val = singleread_spiff();
    ESP_LOGI(TAG, "Read back: %u", val);

    // Burst write/read test
    uint8_t buffer[] = "PatientData:TestRecord123";
    burstwrite_spiff(buffer, sizeof(buffer));

    uint8_t read_buffer[64] = {0};
    int bytes_read = burstread_spiff(read_buffer, 0);
    ESP_LOGI(TAG, "Burst read (%d bytes): %s", bytes_read, read_buffer);

    stop_spiff();
    ESP_LOGI(TAG, "Unit test complete");
}