# OpenHaystack compatible firmware for Moko M1

## Installing prerequisites 

### Get submodules
```
git submodule init
git submodule update
```

### Compile micro-ecc library
```
cd secure_bootloader/micro-ecc-build
make
```

### Generate public/private keypair
```
cd apps/firmware/dfu_images
nrfutil keys generate private.key
nrfutil keys display --key pk --format code private.key --out_file public_key.c
```

### Compile secure_bootloader
```
cd secure_bootloader/build
make
```

### Copy the bootloader to the dfu_images directory
```
cp _build/secure_bootloader_moko.hex ../../firmware/dfu_images
cd ../../
```

### Compile the firmware
```
cd firmware/build
make
cd ../dfu_images
```

### Complete the final image file
Copy the firmware hex to `dfu_images` directory
```
cp ../../firmware/build/_build/nrf52810_xxaa.hex . 
```

Generate a DFU settings page
```
nrfutil settings generate --family NRF52810 --application nrf52810_xxaa.hex --application-version 1 --bootloader-version 1 --bl-settings-version 2 bl_settings.hex
```

Merge the bootloader settings, the bootloader itself, the softdevice and the app
```
mergehex -m bl_settings.hex secure_bootloader_moko.hex nrf52810_xxaa.hex ../../../nRF5_SDK_17.0.2_d674dde/components/softdevice/s112/hex/s112_nrf52_7.2.0_softdevice.hex --output full_dfu.hex
```

Flash the firmware
```
nrfjprog -f nrf52 --recover; while [ $? != 0 ]; do nrfjprog -f nrf52 --recover; done;
nrfjprog -f nrf52 --program full_dfu.hex --reset
```

To generate a bin file of the full firmware you can read it afterwards (this was the better way to "convert" the hex to bin)
```
nrfjprog -f nrf52 --readcode full_dfu.bin
```

### Create a DFU package
Copy the firmware hex to `dfu_images` directory
```
cp ../../firmware/build/_build/nrf52810_xxaa.hex . 
```

Generate package
```
nrfutil pkg generate --hw-version 52 --sd-req 0x0100 --application-version 1 --application nrf52810_xxaa.hex --sd-id 0x103 --key-file private.key app.zip
```