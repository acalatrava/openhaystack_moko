#!/bin/sh

# Exit on error
set -e

# Set working directory
WORKSPACE=/spaceinvader
cd $WORKSPACE

# Make directories for hexadecimal files and the final binary
mkdir $WORKSPACE/hex
mkdir $WORKSPACE/dist

# Initialize and update git submodules
git submodule init
git submodule update

# Compile micro-ecc library
make --directory=$WORKSPACE/apps/secure_bootloader/micro-ecc-build

# Set the public key for the secure bootloader
sed -i "s/\/\* PUBLIC_KEY_PLACEHOLDER \*\//$PUBLIC_KEY_HEX/" $WORKSPACE/apps/firmware/dfu_images/public_key.c

# Compile secure bootloader
make --directory=$WORKSPACE/apps/secure_bootloader/build
mv $WORKSPACE/apps/secure_bootloader/build/_build/secure_bootloader_moko.hex $WORKSPACE/hex

# Compile firmware
make --directory=$WORKSPACE/apps/firmware/build
mv $WORKSPACE/apps/firmware/build/_build/nrf52810_xxaa.hex $WORKSPACE/hex

# Make hexadecimal files
cd $WORKSPACE/hex

# Generate DFU settings
nrfutil settings generate --family NRF52810 --application nrf52810_xxaa.hex --application-version 1 --bootloader-version 1 --bl-settings-version 2 bl_settings.hex

# Merge hex files
cp $WORKSPACE/nRF5_SDK_17.0.2_d674dde/components/softdevice/s112/hex/s112_nrf52_7.2.0_softdevice.hex $WORKSPACE/hex
mergehex --merge bl_settings.hex secure_bootloader_moko.hex nrf52810_xxaa.hex s112_nrf52_7.2.0_softdevice.hex --output firmware.hex

# Convert the HEX file to a BIN file
arm-none-eabi-objcopy --input-target=ihex --output-target=binary firmware.hex $WORKSPACE/dist/firmware.bin
