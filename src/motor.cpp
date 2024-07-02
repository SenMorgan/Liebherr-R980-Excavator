#include "motor.h"
#include <FunctionalInterrupt.h>
#include "pwm_controller.h"

Motor::Motor(uint8_t posMotorPin, uint8_t negMotorPin, bool breakMode, bool reverse)
{
    _posMotorPin = posMotorPin;
    _negMotorPin = negMotorPin;
    _breakMode = breakMode;
    _reverse = reverse;
}

void Motor::setupLimitSwitches(gpio_num_t posLimitPin, gpio_num_t negLimitPin)
{
    if (posLimitPin != GPIO_NUM_NC)
    {
        _posLimitPin = posLimitPin;
        gpio_set_pull_mode(_posLimitPin, GPIO_PULLUP_ONLY);
        attachInterrupt(digitalPinToInterrupt(_posLimitPin), std::bind(&Motor::_handlePosLimitReached, this), CHANGE);
    }
    if (negLimitPin != GPIO_NUM_NC)
    {
        _negLimitPin = negLimitPin;
        gpio_set_pull_mode(_negLimitPin, GPIO_PULLUP_ONLY);
        attachInterrupt(digitalPinToInterrupt(_negLimitPin), std::bind(&Motor::_handleNegLimitReached, this), CHANGE);
    }
}

void IRAM_ATTR Motor::_handlePosLimitReached()
{
    posLimitReached = !gpio_get_level(_posLimitPin);
    if (posLimitReached)
    {
        stopImmediate();
    }
}

void IRAM_ATTR Motor::_handleNegLimitReached()
{
    negLimitReached = !gpio_get_level(_negLimitPin);
    if (negLimitReached)
    {
        stopImmediate();
    }
}

void Motor::stop()
{
    if (_breakMode)
    {
        setMotorPwm(_posMotorPin, _negMotorPin, PWM_ON, PWM_ON);
    }
    else
    {
        setMotorPwm(_posMotorPin, _negMotorPin, PWM_OFF, PWM_OFF);
    }
}

void Motor::stopImmediate()
{
    setMotorPwm(_posMotorPin, _negMotorPin, PWM_OFF, PWM_OFF, true);
}

void Motor::setSpeed(int16_t speed)
{
    if (_reverse)
    {
        speed = -speed;
    }

    if (speed > 0)
    {
        if (!posLimitReached)
        {
            setMotorPwm(_posMotorPin, _negMotorPin, speed, PWM_OFF);
        }
    }
    else if (speed < 0)
    {
        if (!negLimitReached)
        {
            setMotorPwm(_posMotorPin, _negMotorPin, PWM_OFF, -speed);
        }
    }
    else
    {
        stop();
    }
}
