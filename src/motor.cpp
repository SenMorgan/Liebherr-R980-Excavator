#include "Motor.h"
#include <SPI.h>

extern Adafruit_PWMServoDriver pwm; // Assuming pwm is initialized elsewhere

Motor *Motor::_instance = nullptr; // Initialize the static instance pointer

Motor::Motor(uint8_t posMotorPin, uint8_t negMotorPin, bool breakMode, bool reverse)
{
    _posMotorPin = posMotorPin;
    _negMotorPin = negMotorPin;
    _breakMode = breakMode;
    _reverse = reverse;
}

void Motor::setupLimitSwitches(gpio_num_t posLimitPin, gpio_num_t negLimitPin)
{
    _instance = this; // Set the static instance pointer to this instance

    if (posLimitPin != GPIO_NUM_NC)
    {
        _posLimitPin = posLimitPin;
        pinMode(_posLimitPin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(_posLimitPin), posLimitISR, CHANGE);
    }
    if (negLimitPin != GPIO_NUM_NC)
    {
        _negLimitPin = negLimitPin;
        pinMode(_negLimitPin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(_negLimitPin), negLimitISR, CHANGE);
    }
}

void IRAM_ATTR Motor::posLimitISR()
{
    if (_instance)
        _instance->handlePosLimitReached(); // Call the instance-specific handler
}

void IRAM_ATTR Motor::negLimitISR()
{
    if (_instance)
        _instance->handleNegLimitReached(); // Call the instance-specific handler
}

void Motor::handlePosLimitReached()
{
    posLimitReached = !digitalRead(_posLimitPin);
    // TODO: stop the motor if the limit is reached
}

void Motor::handleNegLimitReached()
{
    negLimitReached = !digitalRead(_negLimitPin);
    // TODO: stop the motor if the limit is reached
}

void Motor::setSpeed(int16_t speed)
{
    uint16_t pwmValue = map(abs(speed), 0, 255, 0, 4095);

    if (_reverse)
    {
        speed = -speed;
    }

    if (speed > 0 && !posLimitReached)
    {
        pwm.setPWM(_posMotorPin, 0, pwmValue);
        pwm.setPWM(_negMotorPin, 0, 0);
    }
    else if (speed < 0 && !negLimitReached)
    {
        pwm.setPWM(_posMotorPin, 0, 0);
        pwm.setPWM(_negMotorPin, 0, pwmValue);
    }
    else
    {
        if (_breakMode)
        {
            pwm.setPWM(_posMotorPin, 0, 4095);
            pwm.setPWM(_negMotorPin, 0, 4095);
        }
        else
        {
            pwm.setPWM(_posMotorPin, 0, 0);
            pwm.setPWM(_negMotorPin, 0, 0);
        }
    }
}
