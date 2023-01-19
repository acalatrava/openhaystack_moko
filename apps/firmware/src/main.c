#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ble_stack.h"
#include "openhaystack.h"

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
 * main function
 */
int main(void) {
    // Variable to hold the data to advertise
    uint8_t *ble_address;
    uint8_t *raw_data;
    uint8_t data_len;

    // Set key to be advertised
    data_len = setAdvertisementKey(public_key, &ble_address, &raw_data);

    // Init BLE stack
    init_ble();

    // Set bluetooth address
    setMacAddress(ble_address);

    // Set advertisement data
    setAdvertisementData(raw_data, data_len, ADVERTISING_INTERVAL);

    // Start advertising
    startAdvertisement();

    // Go to low power mode
    while (1) {
        power_manage();
    }
}
