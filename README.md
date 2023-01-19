# openhaystack_moko
This is an internal project for SpaceInvaders to create a OpenHaystack compatible device on a Moko M1 and Moko M2 devices.

## Setting up

### Get submodules
```
git submodule init
git submodule update
```

### Install required dependencies
 - nRF command line tools
 `brew tap homebrew/cask-drivers; brew install --cask nordic-nrf-command-line-tools`

 - binutils
 `brew install binutils`

 - gcc-arm-none-eabi
 `brew install --cask gcc-arm-embedded`

### Compile the firmware
Follow instructions on the `apps` folder