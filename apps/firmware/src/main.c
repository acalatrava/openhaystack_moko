#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "ble_stack.h"
#include "openhaystack.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// Includes for the DFU
#include "nrf_power.h"
#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"
#include "ble_dfu.h"
#include "nrf_bootloader_info.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"

#ifndef uint8_t
    typedef __uint8_t uint8_t;
    typedef __uint16_t uint16_t;
    typedef __uint32_t uint32_t;
    typedef __uint64_t uint64_t;
#endif

/** 
 * advertising interval in milliseconds 
 * 
 * According to power profiler https://devzone.nordicsemi.com/power/w/opp/2/online-power-profiler-for-bluetooth-le
 * average current is 11 µA with a 3000msec interval on a nRF52810
 * so life expectancy of a CR2032 coin-cell (the one used on Moko M1) would be about 1.5 year
 * and life expectancy of a CR2477 coin-cell (the one used on Moko M2) would be about 6 years
 */
#define ADVERTISING_INTERVAL 3000

/**
 * config mode timeout
 * 
 * This setting will specify for how long the device will remain on "config mode" before start
 * advertising the location beacon. Actually you can config the device on "advertising mode" but
 * on "config mode" it will broadcast its name.
 */
#define CONFIG_MODE_TIMEOUT APP_TIMER_TICKS(60000L) // 60 seconds

// Define the "config mode" timer
APP_TIMER_DEF(config_mode_timer);

/**
 * This is the default public key. You can either modify this variable or patch the binary firmware
 * once compiled, which is the preferred way since it can be done without recompilation.
 */
static char public_key[28] = "OFFLINEFINDINGPUBLICKEYHERE!";

//Test1
/*
static char public_key[28] = {0x4e, 0xe3, 0xf3, 0xc5, 0xbf, 0x2f, 0xcb, 0x61, 
                              0x06, 0xc2, 0xb5, 0x1d, 0x80, 0xff, 0x60, 0xb8, 
                              0x77, 0x77, 0x2b, 0xe5, 0xc5, 0xe5, 0x4b, 0x03, 
                              0xaf, 0x76, 0xd5, 0xe1};
*/
//Test2
/*
static char public_key[28] = {0x60, 0x52, 0x77, 0xfe, 0xdc, 0x80, 0xb1, 0x64, 
                              0x4f, 0x9e, 0x16, 0xdf, 0xde, 0x38, 0xeb, 0x4c, 
                              0xd6, 0xaa, 0xf4, 0xee, 0xb3, 0xf6, 0xd5, 0x70, 
                              0x57, 0x3, 0x1, 0x9f};
*/

/**@brief The handler of the config timer
 * 
 * @details this function will be called when the timer timeouts
 */
static void config_timer_handler(void * p_context)
{
    // Variable to hold the data to advertise
    uint8_t *ble_address;
    uint8_t *raw_data;
    uint8_t data_len;

    // Set key to be advertised
    data_len = setAdvertisementKey(public_key, &ble_address, &raw_data);

    // Update advertisement
    updateAdvertisementData(raw_data, data_len);
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{

    // Initialize timer module.
    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&config_mode_timer, 
                                APP_TIMER_MODE_SINGLE_SHOT, 
                                config_timer_handler);
    APP_ERROR_CHECK(err_code);


}

/**@brief Function to start the timers
 * 
 */
static void timers_start(void)
{
       ret_code_t err_code;
       err_code = app_timer_start(config_mode_timer, 
                                    CONFIG_MODE_TIMEOUT, 
                                    NULL);
       APP_ERROR_CHECK(err_code);
}

/**@brief Function for the Power manager.
 */
static void log_init(void)
{
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}
/**
 * main function
 */
int main(void) {
    // Init DFU
    // Initialize the async SVCI interface to bootloader before any interrupts are enabled.
    log_init();

    ret_code_t err_code;
    err_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(err_code);

    // Variable to hold the data to advertise
    uint8_t *ble_address;
    uint8_t *raw_data;

    // Set key to be advertised
    setAdvertisementKey(public_key, &ble_address, &raw_data);

    // Timers
    timers_init();

    // Init BLE stack
    init_ble();

    // DFU
    peer_manager_init();
    gap_params_init();
    gatt_init();

    // Set bluetooth MAC address only if the device is configured
    if (public_key[0] == 'O' &&
        public_key[1] == 'F' &&
        public_key[2] == 'F' &&
        public_key[3] == 'L' &&
        public_key[4] == 'I' &&
        public_key[5] == 'N' &&
        public_key[6] == 'E') {
            // Keep default MAC address
        } else {
            setMacAddress(ble_address);
        }

    advertising_init(ADVERTISING_INTERVAL);

    // Enable services
    services_init();
    conn_params_init();

    // Start timers if the device is configured
    if (public_key[0] == 'O' &&
        public_key[1] == 'F' &&
        public_key[2] == 'F' &&
        public_key[3] == 'L' &&
        public_key[4] == 'I' &&
        public_key[5] == 'N' &&
        public_key[6] == 'E') {
            // Leave unconfigured
        } else {
            timers_start();
        }

    // Start advertising
    startAdvertisement();

    // Go to low power mode
    while (1) {
        power_manage();
    }
}
