#include "configuracoes.hpp"

int configurar_sensores(struct bme280_dev *dev, struct identifier *id) {
    const char path[] = "/dev/i2c-1";

    id->fd = open(path, O_RDWR);

    if(id->fd < 0) {
        // Failed to open the i2c bus
        return 1;
    }

    id->dev_addr = ENDERECO_SENSOR;

    if (ioctl(id->fd, I2C_SLAVE, id->dev_addr) < 0) {
        // Failed to acquire bus access and/or talk to slave
        close(id->fd);
        return 2;
    }

    dev->intf = BME280_I2C_INTF;
    dev->read = user_i2c_read;
    dev->write = user_i2c_write;
    dev->delay_us = user_delay_us;
    dev->intf_ptr = id;

    int rslt = bme280_init(dev);
    if(rslt != BME280_OK) {
        // Failed to initialize the device
        return 3;
    }

    uint8_t settings_sel = 0;

    dev->settings.osr_h = BME280_OVERSAMPLING_1X;
    dev->settings.osr_p = BME280_OVERSAMPLING_16X;
    dev->settings.osr_t = BME280_OVERSAMPLING_2X;
    dev->settings.filter = BME280_FILTER_COEFF_16;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    rslt = bme280_set_sensor_settings(settings_sel, dev);
    if(rslt != BME280_OK) {
        // Failed to set sensor settings
        return 4;
    }

    return 0;
}

int configurar_gpio() {
    if (!bcm2835_init()) {
        return 1;
    }

    // saidas
    bcm2835_gpio_fsel(GPIO_LAMPADA_COZINHA, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(GPIO_LAMPADA_COZINHA, 0);

    bcm2835_gpio_fsel(GPIO_LAMPADA_SALA, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(GPIO_LAMPADA_SALA, 0);

    // entradas
    bcm2835_gpio_fsel(GPIO_SENSOR_SALA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(GPIO_SENSOR_COZINHA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(GPIO_SENSOR_PORTA_COZINHA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(GPIO_SENSOR_JANELA_COZINHA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(GPIO_SENSOR_PORTA_SALA, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(GPIO_SENSOR_JANELA_SALA, BCM2835_GPIO_FSEL_INPT);

    return 0;
}
