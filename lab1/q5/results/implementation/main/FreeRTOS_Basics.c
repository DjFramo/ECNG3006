// Submitted by: Dédjorn Frampton - 816039229
// Q2 - FreeRTOS Basics

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"


// Constraints
#define LED_GPIO 2
#define LOW 0
#define HIGH 1

#define TASK_ONE_DELAY_MS 500
#define TASK_TWO_DELAY_MS 1000
#define TASK_THREE_DELAY_MS 1000



// Variables
const int TASK_ONE_PRIORITY = 2;
const int TASK_TWO_PRIORITY = 1;
const int TASK_THREE_PRIORITY = 3;
SemaphoreHandle_t Mutex = NULL;
TickType_t start;

// Tasks
void task_one(void) {
    while(1) {
        xSemaphoreTake(Mutex, 0);       // Lock Mutex
        gpio_set_level(LED_GPIO, LOW);  // Turn LED ON for active-low LED
        printf("Task 1\n");             // Task 1 indication
        printf("%lu us\n", (unsigned long)esp_timer_get_time());     // Track delay start time
        start = xTaskGetTickCount();

        while (((xTaskGetTickCount() - start) * portTICK_PERIOD_MS) < TASK_ONE_DELAY_MS) {
            // Busy-wait for 0.5 seconds
        }
        
        printf("%lu us\n\n", (unsigned long)esp_timer_get_time());   // Track delay end time
        xSemaphoreGive(Mutex);          // Unlock Mutex
    }
}

void task_two(void) {
    while(1) {
        xSemaphoreTake(Mutex, 0); 
        gpio_set_level(LED_GPIO, HIGH); // Turn LED OFF for active-low LED
        printf("Task 2\n");             // Task 2 indication
        printf("%lu us\n", (unsigned long)esp_timer_get_time());
        start = xTaskGetTickCount();

        while (((xTaskGetTickCount() - start) * portTICK_PERIOD_MS) < TASK_TWO_DELAY_MS) {
            // Busy-wait for 1 second
        }
        
        printf("%lu us\n\n", (unsigned long)esp_timer_get_time());
        xSemaphoreGive(Mutex);
    }
}

void task_three(void) {
    while(1) {
        printf("Task Three is running\n"); // Print status message
        printf("%lu us\n", (unsigned long)esp_timer_get_time());
        start = xTaskGetTickCount();

        while (((xTaskGetTickCount() - start) * portTICK_PERIOD_MS) < TASK_THREE_DELAY_MS) {
            // Busy-wait for 1 second
        }
        
        printf("%lu us\n\n", (unsigned long)esp_timer_get_time());
    }
}


void app_main(void) {

    Mutex = xSemaphoreCreateMutex();

    if (Mutex == NULL) {
        printf("Mutex creation failed\n\n");
    }
    else {
        printf("Mutex created successfully\n\n");
    }

    // Initialize GPIO
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Create tasks
    xTaskCreate((TaskFunction_t)task_one, "Task One", 2048, NULL, TASK_ONE_PRIORITY, NULL);
    xTaskCreate((TaskFunction_t)task_two, "Task Two", 2048, NULL, TASK_TWO_PRIORITY, NULL);
    xTaskCreate((TaskFunction_t)task_three, "Task Three", 2048, NULL, TASK_THREE_PRIORITY, NULL);
}