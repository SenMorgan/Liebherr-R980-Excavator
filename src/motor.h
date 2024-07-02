#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PWMServoDriver.h>

class Motor
{
public:
    Motor(uint8_t posMotorPin, uint8_t negMotorPin, bool breakMode = true, bool reverse = false);
    void setupLimitSwitches(gpio_num_t posLimitPin, gpio_num_t negLimitPin);
    void setSpeed(int16_t speed);
    void stop(void);
    void stopImmediate(void);

    volatile bool posLimitReached;
    volatile bool negLimitReached;

private:
    void _handlePosLimitReached();
    void _handleNegLimitReached();

    uint8_t _posMotorPin;
    uint8_t _negMotorPin;
    gpio_num_t _posLimitPin;
    gpio_num_t _negLimitPin;
    bool _breakMode;
    bool _reverse;
};

#endif