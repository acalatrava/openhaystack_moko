#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "ble_stack.h"
#include "openhaystack.h"

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
 */
#define ADVERTISING_INTERVAL 500

//static char public_key[28] = "OFFLINEFINDINGPUBLICKEYHERE!";
static char public_key[28] = {0x4e, 0xe3, 0xf3, 0xc5, 0xbf, 0x2f, 0xcb, 0x61, 
                              0x06, 0xc2, 0xb5, 0x1d, 0x80, 0xff, 0x60, 0xb8, 
                              0x77, 0x77, 0x2b, 0xe5, 0xc5, 0xe5, 0x4b, 0x03, 
                              0xaf, 0x76, 0xd5, 0xe1};

/**
 * DFU functions
*/

/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            // YOUR_JOB: Get ready to reset into DFU mode
            //
            // If you aren't finished with any ongoing tasks, return "false" to
            // signal to the system that reset is impossible at this stage.
            //
            // Here is an example using a variable to delay resetting the device.
            //
            // if (!m_ready_for_reset)
            // {
            //      return false;
            // }
            // else
            //{
            //
            //    // Device ready to enter
            //    uint32_t err_code;
            //    err_code = sd_softdevice_disable();
            //    APP_ERROR_CHECK(err_code);
            //    err_code = app_timer_stop_all();
            //    APP_ERROR_CHECK(err_code);
            //}
            break;

        default:
            // YOUR_JOB: Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }

    //Power management allowed to reset to DFU mode;
    return true;
}

/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);

static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED)
    {
        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

        //Go to system off.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

/* nrf_sdh state observer. */
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
{
    .handler = buttonless_dfu_sdh_state_observer,
};

void ble_dfu_buttonless_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
            break;
 
        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            break;
 
        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            break;
        default:
            break;
    }
}


/**
 * main function
 */
int main(void) {
    // Init DFU
    // Initialize the async SVCI interface to bootloader before any interrupts are enabled.
    ret_code_t err_code;
    err_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(err_code);

    // Variable to hold the data to advertise
    uint8_t *ble_address;
    uint8_t *raw_data;
    uint8_t data_len;

    // Set key to be advertised
    data_len = setAdvertisementKey(public_key, &ble_address, &raw_data);

    // Init BLE stack
    init_ble();

    // DFU
    peer_manager_init();
    gap_params_init();
    gatt_init();

    // Set bluetooth address
    setMacAddress(ble_address);

    // Set advertisement data
    setAdvertisementData(raw_data, data_len, ADVERTISING_INTERVAL);

    // Enable services
    services_init();
    conn_params_init();

    // Start advertising
    startAdvertisement();

    // Go to low power mode
    while (1) {
        power_manage();
    }
}
