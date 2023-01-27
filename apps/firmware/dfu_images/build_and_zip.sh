#!/bin/sh
cd ../build
make
cd ../dfu_images
cp ../../firmware/build/_build/nrf52810_xxaa.hex .
nrfutil pkg generate --hw-version 52 --sd-req 0x0100 --application-version 1 --application nrf52810_xxaa.hex --sd-id 0x103 --key-file private.key app.zip
