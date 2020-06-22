# mpu9250-mbed

SparkFun's MPU-9250 Arduino Library ported for Mbed OS v5.1x. Supports non blocking operation. Original library can be found here: https://github.com/sparkfun/SparkFun_MPU-9250-DMP_Arduino_Library.

## Configuration

To learn how the Mbed OS configuration system works visit: https://os.mbed.com/docs/mbed-os/v5.15/reference/configuration.html

**Available options**:

* `"mpu9250-mbed.i2c-sda"` - I2C SDA pin
* `"mpu9250-mbed.i2c-scl"` - I2C SCL pin
* `"mpu9250-mbed.i2c-frequency"` - I2C bus frequency (default: 100kHz)
* `"mpu9250-mbed.non-blocking"` - non-blocking operation (default: disabled)
* `"mpu9250-mbed.no-compass"` - no compass support (default: enabled) 

**Example:**

```json
{
    "target_overrides":{
        "*": {
            "mpu9250-mbed.i2c-sda": "PB_11",
            "mpu9250-mbed.i2c-sda": "PB_10",
            "mpu9250-mbed.i2c-frequency": 100000,
            "mpu9250-mbed.non-blocking": 1,
            "mpu9250-mbed.no-compass": 1
        }
    }
}
```