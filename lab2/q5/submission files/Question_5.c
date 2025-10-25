// Submitted by: Dédjorn Frampton - 816039229
// Q5 - Replacing the Mutex with a Semaphore

#include <stdio.h>
#include "FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

// Constraints
#define LED_GPIO 2
#define LOW 0
#define HIGH 1

#define TASK_ONE_DELAY_MS 500
#define TASK_TWO_DELAY_MS 1000
#define TASK_THREE_DELAY_MS 1000

// Logging tags
static const char *TAG_TASK1 = "Task1";
static const char *TAG_TASK2 = "Task2";
static const char *TAG_TASK3 = "Task3";
static const char *TAG_MAIN  = "Main";

// Task Priorities
const int TASK_ONE_PRIORITY = 3;
const int TASK_TWO_PRIORITY = 2;
const int TASK_THREE_PRIORITY = 1;
SemaphoreHandle_t binSem = NULL;

// Task 1: LED ON, busy-wait 0.5s
void task_one(void *pvParameters) {
    while (1) {
        ESP_LOGI(TAG_TASK1, "Attempting to take Binary Semaphore...");

        if (xSemaphoreTake(binSem, portMAX_DELAY)) {
            ESP_LOGI(TAG_TASK1, "Successfully took Binary Semaphore");

            gpio_set_level(LED_GPIO, LOW);  // Active-low LED ON

            if (xSemaphoreGive(binSem) == pdTRUE) {
                ESP_LOGI(TAG_TASK1, "Successfully gave Binary Semaphore");
            } else {
                ESP_LOGW(TAG_TASK1, "Failed to give Binary Semaphore");
            }

            ESP_LOGI(TAG_TASK1, "START: %lu us", (unsigned long)esp_timer_get_time());
            TickType_t start = xTaskGetTickCount();
            while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(TASK_ONE_DELAY_MS)) {
                // Busy-wait for 0.5 seconds
            }
            ESP_LOGI(TAG_TASK1, "END: %lu us", (unsigned long)esp_timer_get_time());

        } else {
            ESP_LOGW(TAG_TASK1, "Failed to take Binary Semaphore");
        }

        ESP_LOGI(TAG_TASK1, "Yielding CPU");
        taskYIELD();  // Explicit yield after busy-wait
    }
}

// Task 2: LED OFF, delay 1s
void task_two(void *pvParameters) {
    while (1) {
        ESP_LOGI(TAG_TASK2, "Attempting to take Binary Semaphore...");

        if (xSemaphoreTake(binSem, portMAX_DELAY)) {
            ESP_LOGI(TAG_TASK2, "Successfully took Binary Semaphore");

            gpio_set_level(LED_GPIO, HIGH);  // Active-low LED OFF

            if (xSemaphoreGive(binSem) == pdTRUE) {
                ESP_LOGI(TAG_TASK2, "Successfully gave Binary Semaphore");
            } else {
                ESP_LOGW(TAG_TASK2, "Failed to give Binary Semaphore");
            }
            
            ESP_LOGI(TAG_TASK2, "START: %lu us", (unsigned long)esp_timer_get_time());
            vTaskDelay(pdMS_TO_TICKS(TASK_TWO_DELAY_MS));
            ESP_LOGI(TAG_TASK2, "END: %lu us", (unsigned long)esp_timer_get_time());

        } else {
            ESP_LOGW(TAG_TASK2, "Failed to take Binary Semaphore");
        }
    }
}

// Task 3: Print status, delay 1s
void task_three(void *pvParameters) {
    while (1) {
        ESP_LOGI(TAG_TASK3, "Running...");

        ESP_LOGI(TAG_TASK3, "START: %lu us", (unsigned long)esp_timer_get_time());
        vTaskDelay(pdMS_TO_TICKS(TASK_THREE_DELAY_MS));
        ESP_LOGI(TAG_TASK3, "END: %lu us", (unsigned long)esp_timer_get_time());
    }
}

void app_main(void) {
    // Create Binary Semaphore
    binSem = xSemaphoreCreateBinary();

    if (binSem == NULL) {
        ESP_LOGE(TAG_MAIN, "Binary Semaphore creation failed");
        return;
    } else {
        ESP_LOGI(TAG_MAIN, "Binary Semaphore created successfully");
        xSemaphoreGive(binSem);  // Initialize the semaphore to 'available'
    }

    // Initialize GPIO
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Create tasks
    xTaskCreate(task_one, "Task One", 2048, NULL, TASK_ONE_PRIORITY, NULL);
    xTaskCreate(task_two, "Task Two", 2048, NULL, TASK_TWO_PRIORITY, NULL);
    xTaskCreate(task_three, "Task Three", 2048, NULL, TASK_THREE_PRIORITY, NULL);
}