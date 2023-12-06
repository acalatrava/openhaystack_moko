#!/bin/sh

# Set working directory
WORKSPACE=/spaceinvader

# Initialize and update git submodules
git submodule init
git submodule update

# Compile micro-ecc library
cd $WORKSPACE/apps/secure_bootloader/micro-ecc-build
make

# Set the public key for the secure bootloader
sed -i "s/\/\* PUBLIC_KEY_PLACEHOLDER \*\//$PUBLIC_KEY_HEX/" $WORKSPACE/apps/firmware/dfu_images/public_key.c

# Compile secure bootloader
cd $WORKSPACE/apps/secure_bootloader/build
make
cp $WORKSPACE/apps/firmware/build/_build/secure_bootloader_moko.hex $WORKSPACE/apps/firmware/dfu_images

# Compile firmware
cd $WORKSPACE/apps/firmware/build
make

# Generate DFU settings
cd $WORKSPACE/apps/firmware/dfu_images
cp $WORKSPACE/apps/firmware/build/_build/nrf52810_xxaa.hex .
nrfutil settings generate --family NRF52810 --application nrf52810_xxaa.hex --application-version 1 --bootloader-version 1 --bl-settings-version 2 bl_settings.hex

# Merge hex files
mergehex -m bl_settings.hex secure_bootloader_moko.hex nrf52810_xxaa.hex ../../../nRF5_SDK_17.0.2_d674dde/components/softdevice/s112/hex/s112_nrf52_7.2.0_softdevice.hex --output full_dfu.hex

# Convert the HEX file to a BIN file
arm-none-eabi-objcopy -I ihex -O binary full_dfu.hex $WORKSPACE/firmware.bin
