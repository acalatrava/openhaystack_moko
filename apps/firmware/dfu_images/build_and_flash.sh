#!/bin/sh
cd ../build
make
cd ../dfu_images
cp ../../firmware/build/_build/nrf52810_xxaa.hex . 
nrfutil settings generate --family NRF52810 --application nrf52810_xxaa.hex --application-version 1 --bootloader-version 1 --bl-settings-version 2 bl_settings.hex
mergehex -m bl_settings.hex secure_bootloader_moko.hex nrf52810_xxaa.hex ../../../nRF5_SDK_17.0.2_d674dde/components/softdevice/s112/hex/s112_nrf52_7.2.0_softdevice.hex --output full_dfu.hex
nrfjprog -f nrf52 --recover; while [ $? != 0 ]; do nrfjprog -f nrf52 --recover; done;
nrfjprog -f nrf52 --program full_dfu.hex --reset
