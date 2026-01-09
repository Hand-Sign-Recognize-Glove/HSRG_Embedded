# esp-idf-mpu-9250

### This is currently WIP

ESP-IDF C driver for [TDK InvenSense MPU9250](https://invensense.tdk.com/products/motion-tracking/9-axis/mpu-9250/) 9-axis gyroscope and accelerometer.

Core MPU-9250 driver is sourced from [libdriver/mpu9250](https://github.com/libdriver/mpu9250) 
and modified to work with the ESP-IDF framework as an IDF component.
* ESP-IDF Version >=5.3 (master)

## Protocols
- [x] I2C
- [ ] SPI

## Features
- [x] Basic sensor reading
- [ ] FIFO
- [ ] Interrupts/ISR
- [ ] DMP
- [ ] I2C Slave devices


## References
* [MPU-9250 Datasheet](https://invensense.tdk.com/download-pdf/icm-42370-p-datasheet/)
* [MPU-9250 Product Specification](https://invensense.tdk.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf)
* [MPU-9250 Register Map](https://invensense.tdk.com/download-pdf/mpu-9250-register-map/)