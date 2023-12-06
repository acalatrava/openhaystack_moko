# OpenHaystack - Moko

This is an internal project developed by [Antonio Calatrava](https://github.com/acalatrava) for [SpaceInvader](https://www.spaceinvader.com/) to create a OpenHaystack compatible device on a Moko M1 and Moko M2 devices.

In November 2023, SpaceInvader open sourced the project under the MIT license so others may benefit and contribute further.

Currently this firmware is compatible with the following models:

- Moko M1-PNDA
- Moko M2-PNDA-HA

This firmware support DFU updating, this way you can generate tags and deploy them over the air.

## Setting up (Mac)

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
