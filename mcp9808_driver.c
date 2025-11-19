#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>

#define DRIVER_NAME "mcp9808_driver" // Driver name
#define MCP9808_TEMP 0x05 // Register for temperature data

/* Global variables */
static struct kobject *temp_kobj; // Kobject for sysfs entry
static struct i2c_client *global_client; // Global variable to store i2c client

/* Function to read temperature from MCP9808 sensor */
static int mcp9808_read_temperature(struct i2c_client *client, int *temp_c, int *temp_f)
{
        int ret;
        u16 temp_raw;

        ret = i2c_smbus_read_word_data(client, MCP9808_TEMP); // Read the temperature register
        if(ret < 0) {
                dev_err(&client->dev, "FAILED TO READ TEMPERATURE DATA\n"); // Error handling
                return ret;
        }

        // Swap the bytes to get the correct temperature data
        temp_raw = ((ret >> 8) | ((ret & 0xFF) << 8));

        *temp_c = ((temp_raw & 0x0FFF) / 16); // Convert to Celsius

        if(temp_raw & 0x1000){
                *temp_c -= 256; // Adjust for negative temperatures
        }

        *temp_f = (*temp_c * 9 / 5) + 32; // Convert Celsius to Fahrenheit

        return 0;
}

/* Show the temperature data in sysfs */
static ssize_t temp_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
        int temp_c, temp_f;
        int ret;

        if(!global_client) {
                return -ENODEV; // Device not available
        }

        ret = mcp9808_read_temperature(global_client, &temp_c, &temp_f); // Read temperature
        if(ret < 0) {
                return ret; // Return error if temperature read fails
        }

        return sprintf(buf, "%dC %dF\n", temp_c, temp_f); // Format and return temperature data
}

/* Define the sysfs attribute for reading temperature */
static struct kobj_attribute temp_attr = __ATTR_RO(temp);

/* Probe function to initialize the sensor */
static int temp_sensor_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        int ret;

        ret = i2c_smbus_read_byte(client); // Test if the sensor is detected
        if(ret < 0) {
                dev_err(&client->dev, "SENSOR NOT DETECTED AT I2C ADDRESS: 0x%02x\n", client->addr); // Error if sensor is not detected
                return -ENODEV;
        }

        dev_info(&client->dev, "Temperature Sensor Probed: I2C Address = 0x%02x\n", client->addr); // Log sensor probe
        global_client = client; // Store client reference

        // Create sysfs directory for the sensor
        temp_kobj = kobject_create_and_add("temperature_sensor", kernel_kobj);
        if(!temp_kobj) {
                dev_err(&client->dev, "FAILED TO CREATE SYSFS DIRECTORY\n"); // Error if sysfs directory creation fails
                return -ENOMEM;
        }

        // Create sysfs file for temperature
        ret = sysfs_create_file(temp_kobj, &temp_attr.attr);
        if(ret) {
                dev_err(&client->dev, "FAILED TO CREATE SYSFS FILE\n"); // Error if sysfs file creation fails
                kobject_put(temp_kobj); // Cleanup created kobject
                return ret;
        }

        dev_info(&client->dev, "Sysfs file and directory created successfully\n"); // Log success

        return 0;
}

/* Remove function to clean up when the sensor is removed */
static int temp_sensor_remove(struct i2c_client *client)
{
        dev_info(&client->dev, "Temperature Sensor Removed: I2C Address: 0x%02x\n", client->addr); // Log sensor removal

        if(temp_kobj) {
                sysfs_remove_file(temp_kobj, &temp_attr.attr); // Remove the sysfs file
                kobject_put(temp_kobj); // Release kobject
        }

        global_client = NULL; // Reset global client reference

        return 0;
}

/* Define the supported I2C device ID */
static const struct i2c_device_id temp_sensor_id[] = {
        { "mcp9808", 0 }, // Device ID for MCP9808 sensor
        { }
};
MODULE_DEVICE_TABLE(i2c, temp_sensor_id); // Register the device ID table

/* Define the I2C driver for the temperature sensor */
static struct i2c_driver temp_sensor_driver = {
        .driver = {
                .name = DRIVER_NAME, // Set driver name
        },
        .probe = temp_sensor_probe, // Probe function
        .remove = temp_sensor_remove, // Remove function
        .id_table = temp_sensor_id, // Device ID table
};

/* Initialize the driver */
static int __init mcp9808_init(void) {
        return i2c_add_driver(&temp_sensor_driver); // Register the I2C driver
}

/* Cleanup the driver */
static void __exit mcp9808_exit(void) {
        i2c_del_driver(&temp_sensor_driver); // Deregister the I2C driver
}

module_init(mcp9808_init); // Module entry point
module_exit(mcp9808_exit); // Module exit point

MODULE_LICENSE("GPL"); // Define the module license
MODULE_AUTHOR("BHUVAN"); // Define the author of the module
MODULE_DESCRIPTION("Simple Temperature Sensor Driver"); // Module description
