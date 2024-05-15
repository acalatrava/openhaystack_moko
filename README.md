# openhaystack_moko
This is an internal project developed by Antonio Calatrava for SpaceInvader Europe Aps to create a OpenHaystack compatible device on a Moko M1 and Moko M2 devices.

In November 2023, SpaceInvader Europe Aps open sourced the project under the MIT license so others may benefit and contribute further.

In April 2024, [University of Zurich](https://www.uzh.ch/en.html) requested me to make it compatible with the [C2 beacon from KKM](https://www.kkmcn.com/thin-beacon-tag-c2) which is based on nRF52805 chip

This branch has been tested with such beacon with successful results.

## Key Features
- **Energy Efficiency**: The firmware is optimized for extended battery life. Devices are set to enter a deep sleep state immediately upon activation and will only start advertising when the button is pressed.
- **User Interaction**: Integrated support for a button and LED allows for manual control to turn the device on or off when not configured.
- **Device Updates**: Supports Device Firmware Update (DFU). New firmware versions can be generated with a specific public key and flashed via DFU to reconfigure the device.

## Configuration
To update or configure the firmware:
1. Generate the firmware with the required public key.
2. Use the DFU process to flash this firmware onto the device.

### Note
This firmware is specifically designed for a device with a permanently soldered battery to maximize the battery's lifespan and reliability.


### Compile the firmware

Follow instructions in the `apps/firmware` folders.

### Compile via Docker

I added the nice Docker build from the forked https://github.com/SpaceInvaderTech/openhaystack_moko repository

    docker build --platform linux/amd64 --tag open-tag .
    docker run --rm --platform linux/amd64 --env "PUBLIC_KEY_HEX=0x00" open-tag
