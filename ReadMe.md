# OpenHaystack - Moko

This is an internal project developed by [Antonio Calatrava](https://github.com/acalatrava) for [SpaceInvader](https://www.spaceinvader.com/) to create a OpenHaystack compatible device on a Moko M1 and Moko M2.

In November 2023, SpaceInvader open sourced the project under the MIT license.

Currently the firmware is compatible with the following models:

- Moko M1-PNDA
- Moko M2-PNDA-HA

The firmware support DFU updating.

### Compile the firmware

Follow instructions in the `apps` and `apps/firmware` folders.

### Compile via Docker

    docker build --platform linux/amd64 --tag si-moko .
    docker run --rm --platform linux/amd64 --env "PUBLIC_KEY_HEX=0x00" si-moko
