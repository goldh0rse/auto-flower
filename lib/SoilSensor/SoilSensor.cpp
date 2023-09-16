#include "SoilSensor.h"

Adafruit_seesaw::Adafruit_seesaw(TwoWire *i2c_bus) {
    if (i2c_bus == NULL) {
        _i2cbus = &Wire;
    } else {
        _i2cbus = i2c_bus;
    }
}

bool Adafruit_seesaw::begin(uint8_t addr, int8_t flow, bool reset) {
    _flow = flow;

    if (_flow != -1)
        ::pinMode(_flow, INPUT);

    if (_i2c_dev) {
        delete _i2c_dev;
    }

    _i2c_dev = new Adafruit_I2CDevice(addr, _i2cbus);

    bool found = false;
    for (int retries = 0; retries < 10; retries++) {
        if (_i2c_dev->begin()) {
            found = true;
            break;
        }
        delay(10);
    }

    if (!found) {
        return false;
    }

    if (reset) {
        found = false;
        SWReset();
        for (int retries = 0; retries < 10; retries++) {
            if (_i2c_dev->detected()) {
                found = true;
                break;
            }
            delay(10);
        }
    }

    if (!found) {
        return false;
    }

    found = false;
    for (int retries = 0; !found && retries < 10; retries++) {
        uint8_t c = 0;

        this->ss_read(SEESAW_STATUS_BASE, SEESAW_STATUS_HW_ID, &c, 1);
        if ((c == SEESAW_HW_ID_CODE_SAMD09) ||
            (c == SEESAW_HW_ID_CODE_TINY817) ||
            (c == SEESAW_HW_ID_CODE_TINY807) ||
            (c == SEESAW_HW_ID_CODE_TINY816) ||
            (c == SEESAW_HW_ID_CODE_TINY806) ||
            (c == SEESAW_HW_ID_CODE_TINY1616) ||
            (c == SEESAW_HW_ID_CODE_TINY1617)) {
            found = true;
            _hardwaretype = c;
        }

        delay(10);
    }

    return found;
}

bool Adafruit_seesaw::SWReset() {
    return this->write8(SEESAW_STATUS_BASE, SEESAW_STATUS_SWRST, 0xFF);
}

void Adafruit_seesaw::ss_pinMode(uint8_t pin, uint8_t mode) {
    if (pin >= 32)
        ss_pinModeBulk(0, 1ul << (pin - 32), mode);
    else
        ss_pinModeBulk(1ul << pin, mode);
}

void Adafruit_seesaw::ss_pinModeBulk(uint32_t pins, uint8_t mode) {
    uint8_t cmd[] = {(uint8_t)(pins >> 24),
                     (uint8_t)(pins >> 16),
                     (uint8_t)(pins >> 8),
                     (uint8_t)pins};
    switch (mode) {
    case OUTPUT:
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRSET_BULK, cmd, 4);
        break;
    case INPUT:
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
        break;
    case INPUT_PULLUP:
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4);
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 4);
        break;
    case INPUT_PULLDOWN:
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 4);
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 4);
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_CLR, cmd, 4);
        break;
    }
}

void Adafruit_seesaw::ss_pinModeBulk(uint32_t pinsa,
                                     uint32_t pinsb,
                                     uint8_t mode) {
    uint8_t cmd[] = {(uint8_t)(pinsa >> 24),
                     (uint8_t)(pinsa >> 16),
                     (uint8_t)(pinsa >> 8),
                     (uint8_t)pinsa,
                     (uint8_t)(pinsb >> 24),
                     (uint8_t)(pinsb >> 16),
                     (uint8_t)(pinsb >> 8),
                     (uint8_t)pinsb};
    switch (mode) {
    case OUTPUT:
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRSET_BULK, cmd, 8);
        break;
    case INPUT:
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 8);
        break;
    case INPUT_PULLUP:
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 8);
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 8);
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_SET, cmd, 8);
        break;
    case INPUT_PULLDOWN:
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_DIRCLR_BULK, cmd, 8);
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_PULLENSET, cmd, 8);
        this->write(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK_CLR, cmd, 8);
        break;
    }
}

bool Adafruit_seesaw::ss_read(uint8_t regHigh,
                              uint8_t regLow,
                              uint8_t *buf,
                              uint8_t num,
                              uint16_t delay) {
    uint8_t pos = 0;
    uint8_t prefix[2];
    prefix[0] = (uint8_t)regHigh;
    prefix[1] = (uint8_t)regLow;

    // on arduino we need to read in 32 byte chunks
    while (pos < num) {
        uint8_t read_now = min(32, num - pos);

        if (_flow != -1) {
            while (!::digitalRead(_flow))
                yield();
        }

        if (!_i2c_dev->write(prefix, 2)) {
            return false;
        }

        // TODO: tune this
        delayMicroseconds(delay);

        if (_flow != -1) {
            while (!::digitalRead(_flow))
                yield();
        }

        if (!_i2c_dev->read(buf + pos, read_now)) {
            return false;
        }
        pos += read_now;
    }
    return true;
}

bool Adafruit_seesaw::ss_digitalRead(uint8_t pin) {
    if (pin >= 32)
        return ss_digitalReadBulkB((1ul << (pin - 32))) != 0;
    else
        return ss_digitalReadBulk((1ul << pin)) != 0;
}

uint32_t Adafruit_seesaw::ss_digitalReadBulkB(uint32_t pins) {
    uint8_t buf[8];
    this->ss_read(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buf, 8);
    uint32_t ret = ((uint32_t)buf[4] << 24) | ((uint32_t)buf[5] << 16) |
        ((uint32_t)buf[6] << 8) | (uint32_t)buf[7];
    return ret & pins;
}

uint32_t Adafruit_seesaw::ss_digitalReadBulk(uint32_t pins) {
    uint8_t buf[4];
    this->ss_read(SEESAW_GPIO_BASE, SEESAW_GPIO_BULK, buf, 4);
    uint32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
        ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
    return ret & pins;
}

bool Adafruit_seesaw::write(uint8_t regHigh,
                            uint8_t regLow,
                            uint8_t *buf = NULL,
                            uint8_t num = 0) {
    uint8_t prefix[2];
    prefix[0] = (uint8_t)regHigh;
    prefix[1] = (uint8_t)regLow;

    if (_flow != -1)
        while (!::digitalRead(_flow))
            yield();

    if (!_i2c_dev->write(buf, num, true, prefix, 2)) {
        return false;
    }

    return true;
}

size_t Adafruit_seesaw::write(uint8_t character) {
    // TODO: add support for multiple sercoms
    this->write8(SEESAW_SERCOM0_BASE, SEESAW_SERCOM_DATA, character);
    delay(
        1); // TODO: this can be optimized... it's only needed for longer writes
    return 1;
}

size_t Adafruit_seesaw::write(const char *str) {
    uint8_t buf[32];
    uint8_t len = 0;
    while (*str) {
        buf[len] = *str;
        str++;
        len++;
    }
    this->write(SEESAW_SERCOM0_BASE, SEESAW_SERCOM_DATA, buf, len);
    return len;
}

bool Adafruit_seesaw::write8(byte regHigh, byte regLow, byte value) {
    return this->write(regHigh, regLow, &value, 1);
}

uint16_t Adafruit_seesaw::ss_touchRead(uint8_t pin) {
    uint8_t buf[2];
    uint8_t p = pin;
    uint16_t ret = 65535;

    for (uint8_t retry = 0; retry < 5; retry++) {
        if (this->ss_read(SEESAW_TOUCH_BASE,
                          SEESAW_TOUCH_CHANNEL_OFFSET + p,
                          buf,
                          2,
                          3000 + retry * 1000)) {
            ret = ((uint16_t)buf[0] << 8) | buf[1];
            break;
        }
    }
    return ret;
}

float Adafruit_seesaw::getTemp() {
    uint8_t buf[4];
    this->ss_read(SEESAW_STATUS_BASE, SEESAW_STATUS_TEMP, buf, 4, 1000);
    int32_t ret = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
        ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
    return (1.0 / (1UL << 16)) * ret;
}