#include "ble.h"
#include "ble_dfu.h"
#include <stdbool.h>
#include <stdint.h>

#ifndef uint8_t
    typedef __uint8_t uint8_t;
    typedef __uint16_t uint16_t;
    typedef __uint32_t uint32_t;
    typedef __uint64_t uint64_t;
#endif

/*  
 *  init_ble will initialize the ble stack, it will use the crystal definition based on NRF_CLOCK_LFCLKSRC. 
 *  In devices with no external crystal you should use the internal rc. You can look at the e104bt5032a_board.h file
 */
void init_ble();

/**
 * setMacAddress will set the bluetooth address
 */
void setMacAddress(uint8_t *addr);

/**
 * setAdvertisementData will set the data to be advertised
 * 
 * @param[in] data raw data to be advertised
 * @param[in] dlen length of the data
 */
void setAdvertisementData(uint8_t *data, uint8_t dlen);

/**
 * updateAdvertisementData will update the data to be advertised
 * 
 * @param[in] data raw data to be advertised
 * @param[in] dlen length of the data
 */
void updateAdvertisementData(uint8_t *data, uint8_t dlen);

/**
 * Start advertising
 */
void startAdvertisement();

/**
 * Initialize advertising with specified interval in msec
 */
void advertising_init(int interval);

/**
 * Function for the Power manager.
 */
void power_manage(void);

/**
 * DFU required functions
*/
void peer_manager_init();
void gap_params_init(void);
void gatt_init(void);
void services_init(void);
void conn_params_init(void);
void ble_dfu_buttonless_evt_handler(ble_dfu_buttonless_evt_type_t event);