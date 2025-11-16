// Submitted by: Dédjorn Frampton - 816039229
// Q3 - Unit Test for Non-volatile Storage Routines with Timing

#include "SPIFFS_Routines.h"
#include "esp_timer.h"   // For high-resolution timing

#define TAG "SPIFFS_TEST"
#define NUM_ITERATIONS 50   // Number of repetitions for averaging

void app_main(void) {
    ESP_LOGI(TAG, "Starting SPIFFS timing unit test...");

    if (!init_spiff()) {
        ESP_LOGE(TAG, "SPIFFS initialization failed");
        return;
    }

    // Burst write/read test
    uint8_t buffer[] = "PatientData:TestRecord123";
    burstwrite_spiff(buffer, sizeof(buffer));

    uint8_t read_buffer[64] = {0};
    int bytes_read = burstread_spiff(read_buffer, 0);
    ESP_LOGI(TAG, "Burst read (%d bytes): %s", bytes_read, read_buffer);

    
    // Variables to track timing stats
    int64_t start, end, duration;
    int64_t total_write = 0, max_write = 0;
    int64_t total_read = 0, max_read = 0;

    // Repeat multiple iterations to calculate average and max
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // --- Single Write Test ---
        start = esp_timer_get_time();
        singlewrite_spiff(12345 + i);   // Write different values each time
        end = esp_timer_get_time();
        duration = end - start;
        total_write += duration;
        if (duration > max_write) max_write = duration;
        ESP_LOGI(TAG, "Iteration %d: singlewrite_spiff took %lld us", i, duration);

        // --- Single Read Test ---
        start = esp_timer_get_time();
        uint32_t val = singleread_spiff();
        end = esp_timer_get_time();
        duration = end - start;
        total_read += duration;
        if (duration > max_read) max_read = duration;
        ESP_LOGI(TAG, "Iteration %d: singleread_spiff took %lld us (value=%u)", i, duration, val);
    }

    // Calculate averages
    int64_t avg_write = total_write / NUM_ITERATIONS;
    int64_t avg_read = total_read / NUM_ITERATIONS;

    ESP_LOGI(TAG, "————— Timing Summary —————");
    ESP_LOGI(TAG, "singlewrite_spiff: avg = %lld us, max = %lld us", avg_write, max_write);
    ESP_LOGI(TAG, "singleread_spiff:  avg = %lld us, max = %lld us", avg_read, max_read);

    stop_spiff();
    ESP_LOGI(TAG, "Unit test complete");
}