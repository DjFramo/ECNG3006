// Submitted by: Dédjorn Frampton - 816039229
// Q7 - Finite State Machine Implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/hw_timer.h" // Include hw_timer for timer functionalities

#include "esp_log.h"

#define EX_UART_NUM UART_NUM_0
#define BUF_SIZE (1024)
#define ON 1
#define OFF 0

#define LED_GPIO 2 // Example GPIO for LED

#define RELOAD    true         // hw_timer will use auto reload
#define TICK_PERIOD_MS   10
#define DEBOUNCE_MS     500

unsigned int state;
uint8_t old_char = 0, current_char = 0;
volatile int tick_counter = 0;

void hw_timer_callback(void *arg) {
    tick_counter += TICK_PERIOD_MS;
}


//TickType_t last_event_tick = 0;
void on() {
    gpio_set_level(LED_GPIO, 1);
    tick_counter = 0; 

}

void off() {
    gpio_set_level(LED_GPIO, 0);
    tick_counter = 0; 
}

void app_main() {

    ESP_LOGI("FSM", "App started");

    // UART setup
    uart_config_t uart_config = {
        .baud_rate = 74880,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(EX_UART_NUM, &uart_config);
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Timer setup
    hw_timer_init(hw_timer_callback, NULL);
    hw_timer_alarm_us(TICK_PERIOD_MS * 1000, RELOAD);
    ESP_LOGI("TIMER", "Timer started");

    // GPIO setup for LED
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    state = OFF;
    off();

    uint8_t data[1];

    while (1) {
        int len = uart_read_bytes(EX_UART_NUM, data, 1, 0 ); //could use 100 / portTICK_PERIOD_MS instead. 
        if (len > 0) {
            old_char = current_char;
            current_char = data[0];

            // Uses if-else technique for FSM implementation.
            // This allows for debouncing of the input character within 500ms
            // or if a different character is received.
            // State remains the same otherwise.
            if (state == ON) {
                if ((tick_counter > DEBOUNCE_MS) && (current_char == old_char)) {
                    state = OFF;
                    off(); 
                }
                else if (current_char != old_char) {
                    state = OFF;
                    off();
                }
                else {
                    // stay in ON state. If the same character is received within 500ms it must be ignored
                }
            }
            else if (state == OFF) {
                if ((tick_counter > DEBOUNCE_MS) && (current_char == old_char)) {
                    state = ON;
                    on();
                }
                else if (current_char != old_char) {
                    state = ON;
                    on();
                }
                else {
                    // stay in OFF state. If the same character is received within 500ms it must be ignored
                }
            }
        }
        else {
            current_char = 0; // No input received
        }
        
        vTaskDelay(pdMS_TO_TICKS(TICK_PERIOD_MS)); // Small delay to avoid busy loop
    
    }
}