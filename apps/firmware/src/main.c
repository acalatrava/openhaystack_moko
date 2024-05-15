#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "app_error.h"

// BLE includes
#include "openhaystack.h"
#include "ble_stack.h"
#include "ble_dfu.h"
#include "nrf_dfu_ble_svci_bond_sharing.h"

// Logging includes
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// Power management includes
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "nrf_bootloader_info.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"

// Timers includes
#include "app_timer.h"
#include "nrf_delay.h"

// Device IO includes
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"
#include "SEGGER_RTT.h"

// Application-specific configuration
#define BUTTON_PIN 5
#define LED_PIN 4
#define LONG_PRESS_TIME APP_TIMER_TICKS(2000)      // 2 seconds
#define ADVERTISING_INTERVAL 3000                  // milliseconds
#define CONFIG_MODE_TIMEOUT APP_TIMER_TICKS(60000) // 60 seconds

// Timers declaration
APP_TIMER_DEF(config_timer_id);
APP_TIMER_DEF(button_timer_id);

// Device status flags
static bool button_down = false;
static bool app_running = false;
static bool restart_on_release = false;

// Default public key
static char public_key[28] = "OFFLINEFINDINGPUBLICKEYHERE!";

// Custom logger function
#define log(format, ...)                                          \
    do                                                            \
    {                                                             \
        char _buf[128];                                           \
        snprintf(_buf, sizeof(_buf), format "\n", ##__VA_ARGS__); \
        SEGGER_RTT_WriteString(0, _buf);                          \
    } while (0)

// Forward declarations
void led_turn_on(void);
void led_turn_off(void);

/**
 * @brief Functions for handling the button press.
 */
// Button timer handler called when the button is pressed for a long time
void button_timer_handler(void *p_context)
{
    log("Button timer handler");

    if (button_down)
    {
        log("Long press detected");
        if (app_running)
        {
            // Turn ON LED so the user knows the device is turning off
            nrf_gpio_pin_set(LED_PIN);

            // The device will restart when the button is released. This is
            // because if you restart the device with the button pressed
            // the release event will be interpreted as a new press.
            restart_on_release = true;
        }
        else
        {
            led_turn_on();
            app_running = true;
        }
    }
}

// Button configuration
void config_button_timer(void)
{
    ret_code_t err_code = app_timer_create(&button_timer_id, APP_TIMER_MODE_SINGLE_SHOT, button_timer_handler);
    APP_ERROR_CHECK(err_code);
}

// Button handler called when the button is pressed
void button_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    log("Button handler pressed %lu %i", pin, action);
    ret_code_t err_code;

    if (pin == BUTTON_PIN)
    {
        button_down = !button_down;
        log(button_down ? "Start timer" : "Stop timer");

        if (button_down)
        {
            err_code = app_timer_start(button_timer_id, LONG_PRESS_TIME, NULL);
        }
        else
        {
            err_code = app_timer_stop(button_timer_id);
            if (restart_on_release)
            {
                // System needs to be restarted because the button was pressed for long time
                NVIC_SystemReset();
            }
        }
        APP_ERROR_CHECK(err_code);
    }
}

// Button initialization
void button_init(void)
{
    config_button_timer();
    ret_code_t err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
    in_config.pull = NRF_GPIO_PIN_PULLUP;
    in_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;

    log("Button init1");
    err_code = nrf_drv_gpiote_in_init(BUTTON_PIN, &in_config, button_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(BUTTON_PIN, true);

    log("Button initialized");
}

/**
 * @brief Functions for handling the LED.
 */

// Function to show device is turning on, LED is flashing 3 times
void led_turn_on(void)
{
    log("LED on");
    for (int i = 0; i < 3; i++)
    {
        nrf_gpio_pin_set(LED_PIN);
        nrf_delay_ms(100);
        nrf_gpio_pin_clear(LED_PIN);
        nrf_delay_ms(100);
    }
}

// Function to show device is turning off, LED is flashing 1 long time
void led_turn_off(void)
{
    log("LED off");
    nrf_gpio_pin_set(LED_PIN);
    nrf_delay_ms(1000);
    nrf_gpio_pin_clear(LED_PIN);
}

// LED initialization
void led_init(void)
{
    log("LED initialized");
    nrf_gpio_cfg_output(LED_PIN);
}

/**
 * @brief Functions for handling the timers.
 */
static void config_timer_handler(void *p_context)
{
    uint8_t *ble_address, *raw_data;
    uint8_t data_len = setAdvertisementKey(public_key, &ble_address, &raw_data);
    updateAdvertisementData(raw_data, data_len);
}

static void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&config_timer_id, APP_TIMER_MODE_SINGLE_SHOT, config_timer_handler);
    APP_ERROR_CHECK(err_code);
}

static void timers_start(void)
{
    ret_code_t err_code = app_timer_start(config_timer_id, CONFIG_MODE_TIMEOUT, NULL);
    APP_ERROR_CHECK(err_code);
}

// Logging initialization
static void log_init(void)
{
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    log_init();
    log("Main start");

    led_init();
    button_init();

    ret_code_t err_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(err_code);

    // Variable to hold the data to advertise
    uint8_t *ble_address;
    uint8_t *raw_data;

    // Set key to be advertised
    setAdvertisementKey(public_key, &ble_address, &raw_data);

    // Initialize timers
    timers_init();

    // Initialize BLE
    init_ble();

    // DFU service
    peer_manager_init();
    gap_params_init();
    gatt_init();

    // Set bluetooth address
    if (memcmp(public_key, "OFFLINE", 7) != 0)
        setMacAddress(ble_address);

    // Initialize advertising
    advertising_init(ADVERTISING_INTERVAL);

    // Initialize services
    services_init();
    conn_params_init();

    if (memcmp(public_key, "OFFLINE", 7) != 0)
    {
        // If device is configured (aka public key is set), we will start advertising from the start
        timers_start();
        led_turn_on();
        app_running = true;
    }
    else
    {
        led_turn_off();
    }

    while (1)
    {
        if (app_running)
        {
            startAdvertisement();
            while (1)
            {
                power_manage();
            }
        }
        power_manage();
    }
}
