# MCP9808 I2C Temperature Sensor Driver

## Description
This repository contains a Linux kernel module that interfaces with the MCP9808 I2C temperature sensor. The module reads and processes temperature data from the sensor and provides access to the temperature readings via the sysfs interface.

## Features
- Communicates with the MCP9808 sensor over I2C.
- Reads temperature data from the sensor.
- Exposes temperature readings via the sysfs interface.
- Supports both Celsius and Fahrenheit temperature readings.

## Files
- `mcp9808_driver.c` - The main kernel module source code.
- `mcp9808-overlay.dts` - Device tree overlay for enabling the MCP9808 sensor.
- `Makefile` - Build instructions for compiling the kernel module.

## Requirements
- Linux kernel headers installed on your system.
- An MCP9808 temperature sensor connected via I2C.

## Compilation and Installation
To build the module, run:
```sh
make
```
To insert the module into the kernel:
```sh
sudo insmod mcp9808_driver.ko
```
To remove the module:
```sh
sudo rmmod mcp9808_driver
```
To clean the build files:
```sh
make clean
```

## Usage
Once the module is loaded, temperature data can be accessed via sysfs:
```sh
cat /sys/kernel/temperature_sensor/temp
```
This will output the temperature in both Celsius and Fahrenheit.

## License
This project is licensed under the GPL v2 license.

