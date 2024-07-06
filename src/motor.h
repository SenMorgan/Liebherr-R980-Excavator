#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PWMServoDriver.h>

class Motor
{
public:
    Motor(uint8_t posMotorPin, uint8_t negMotorPin, bool breakMode = true, bool reverse = false);
    void setupLimitSwitches(gpio_num_t posLimitPin, gpio_num_t negLimitPin, uint32_t debounceTime = 50);
    void updateLimitSwitches(bool stopOnLimit = true);
    void setSpeed(int16_t speed);
    void stop(void);
    void stopImmediate(void);

    // Limit switch flags
    bool posLimitReached, negLimitReached;

private:
    // Limit switch interrupt handlers
    void _handlePosLimitReached();
    void _handleNegLimitReached();

    // Motor variables
    uint8_t _posMotorPin, _negMotorPin;
    bool _breakMode;
    bool _reverse;

    // Limit switch variables
    gpio_num_t _posLimitPin, _negLimitPin;
    bool _lastPosLimitState, _lastNegLimitState;
    volatile bool _posLimitState, _negLimitState;
    uint32_t _lastPosLimitTriggerTime, _lastNegLimitTriggerTime;
    uint32_t _debounceTime;
};

#endif