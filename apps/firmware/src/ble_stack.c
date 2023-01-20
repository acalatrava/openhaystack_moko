#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_stack.h"

#include "nordic_common.h"
#include "bsp.h"
#include "nrf_soc.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "ble_advdata.h"
#include "app_timer.h"
#include "nrf_pwr_mgmt.h"

#ifndef uint8_t
    typedef __uint8_t uint8_t;
    typedef __uint16_t uint16_t;
    typedef __uint32_t uint32_t;
    typedef __uint64_t uint64_t;
#endif

#define APP_BLE_CONN_CFG_TAG            1
#define CFG_BLE_TX_POWER_LEVEL          4

enum
{
  CONN_CFG_PERIPHERAL = 1,
  CONN_CFG_CENTRAL = 2,
};

static uint8_t              m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

/*******************************************************************************
 *   BLE stack specific functions
 ******************************************************************************/

/*  
 *  init_ble will initialize the ble stack, it will use the crystal definition based on NRF_CLOCK_LFCLKSRC. 
 *  In devices with no external crystal you should use the internal rc. You can look at the e104bt5032a_board.h file
 */
void init_ble() {
    ret_code_t err_code;

    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);
}

/**
 * setMacAddress will set the bluetooth address
 */
void setMacAddress(uint8_t *addr) {
    ret_code_t err_code;

    ble_gap_addr_t gap_addr;
    gap_addr.addr_id_peer = 0;
    gap_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;

    memcpy(gap_addr.addr, addr, sizeof(gap_addr.addr));
    err_code = sd_ble_gap_addr_set(&gap_addr);
    APP_ERROR_CHECK(err_code);
}

/**
 * setAdvertisementData will set the data to be advertised
 */
void setAdvertisementData(uint8_t *data, uint8_t dlen, int interval) {
    ret_code_t err_code;

    static ble_gap_adv_data_t gap_adv =
        {
            .adv_data      = { .p_data = NULL, .len = 0 },
            .scan_rsp_data = { .p_data = NULL, .len = 0 } // No scan response data needed
        };

    gap_adv.adv_data.p_data = data;
    gap_adv.adv_data.len = dlen;

    ble_gap_adv_params_t adv_para =
        {
            .properties    = { .type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED, .anonymous  = 0 },
            .p_peer_addr   = NULL                                        , // Undirected advertisement
            .interval      = MSEC_TO_UNITS(interval, UNIT_0_625_MS)      , // advertising interval (in units of 0.625 ms)
            .duration      = 0                                           , // in 10-ms unit

            .max_adv_evts  = 0                        , // TODO can be used for fast/slow mode
            .channel_mask  = { 0, 0, 0, 0, 0 }        , // 40 channel, set 1 to disable
            .filter_policy = BLE_GAP_ADV_FP_ANY       ,

            .primary_phy   = BLE_GAP_PHY_AUTO         , // 1 Mbps will be used
            .secondary_phy = BLE_GAP_PHY_AUTO         , // 1 Mbps will be used
            // , .set_id, .scan_req_notification
        };

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &gap_adv, &adv_para);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, CFG_BLE_TX_POWER_LEVEL);
    APP_ERROR_CHECK(err_code);
}

/**
 * Start advertising at the specified interval
 * 
 * @param[in] interval advertising interval in milliseconds
 */
void startAdvertisement(int interval) {
    ret_code_t err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, CONN_CFG_PERIPHERAL);
    APP_ERROR_CHECK(err_code);
}

/**
 * Function for the Power manager.
 */
void power_manage(void)
{
    // https://devzone.nordicsemi.com/f/nordic-q-a/75296/nrf_pwr_mgmt_run-vs-sd_app_evt_wait
    //uint32_t err_code = sd_app_evt_wait();
    //APP_ERROR_CHECK(err_code);

    nrf_pwr_mgmt_run();
}
