# Liebherr R980 Excavator Controller

This project is a controller for the Liebherr R980 SME excavator model by SIKU. It is designed to be used with an ESP32 microcontroller and a custom PCB. The controller is capable of controlling the excavator's movements and functions, such as driving, rotating, and moving the arm and bucket. The controller drives the excavator's motors and lights using PCA9685 PWM driver and DRV8833 motor driver.

The project was created in PlatformIO 15.06.2024

[![ESP32](https://img.shields.io/badge/ESP-32-000000.svg?longCache=true&style=flat&colorA=AA101F)](https://www.espressif.com/en/products/socs/esp32)<br>
[![Build with PlatformIO](https://img.shields.io/badge/Build%20with-PlatformIO-orange)](https://platformio.org/)<br>
[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

<br>

## Configuration and build
1. Ensure you have the [PlatformIO](https://platformio.org/) extension installed in VS Code.
2. Create a copy of `platformio_override.ini.example` and rename it to `platformio_override.ini`.
3. Customize the values in `platformio_override.ini`.
4. Modify hardware and other settings in the `include\constants.h` file if necessary.
5. Use PlatformIO to build and upload the project to your ESP device.

## Dependencies
All dependencies could be found in `platformio.ini` file under `lib_deps` section.

## Copyright
Copyright (c) 2024 Sen Morgan. Licensed under the MIT license, see LICENSE.md