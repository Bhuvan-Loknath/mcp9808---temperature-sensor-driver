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
Once the module is loaded, you need to manually register the MCP9808 sensor on the I2C bus:
```sh
echo mcp9808 0x18 | sudo tee /sys/bus/i2c/devices/i2c-1/new_device
```
After registering the device, temperature data can be accessed via sysfs:
```sh
cat /sys/kernel/temperature_sensor/temp
```
This will output the temperature in both Celsius and Fahrenheit.

## Automating Sensor Registration (Optional)
To avoid manually registering the sensor after each reboot, add the following command to `/etc/rc.local` **before** `exit 0`:
```sh
echo mcp9808 0x18 | sudo tee /sys/bus/i2c/devices/i2c-1/new_device
```
Then, make `/etc/rc.local` executable if it isn’t already:
```sh
sudo chmod +x /etc/rc.local
```
This ensures that the MCP9808 sensor is automatically registered at boot.

## License
This project is licensed under the GPL v2 license.
