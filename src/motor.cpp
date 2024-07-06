#include "motor.h"
#include <FunctionalInterrupt.h>
#include "pwm_controller.h"

/**
 * @brief Construct a new Motor object.
 *
 * @param posMotorPin The GPIO pin number of the positive motor terminal.
 * @param negMotorPin The GPIO pin number of the negative motor terminal.
 * @param breakMode Flag indicating whether to use braking mode.
 * @param reverse Flag indicating whether to reverse the motor direction.
 */
Motor::Motor(uint8_t posMotorPin, uint8_t negMotorPin, bool breakMode, bool reverse)
{
    _posMotorPin = posMotorPin;
    _negMotorPin = negMotorPin;
    _breakMode = breakMode;
    _reverse = reverse;
}

/**
 * @brief Setup the limit switches for the motor.
 *
 * @param posLimitPin The GPIO pin number of the positive limit switch.
 * @param negLimitPin The GPIO pin number of the negative limit switch.
 * @param debounceTime The debounce time in milliseconds.
 */
void Motor::setupLimitSwitches(gpio_num_t posLimitPin, gpio_num_t negLimitPin, uint32_t debounceTime)
{
    // Set the debounce time
    _debounceTime = debounceTime;

    // Set the limit pins
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

/**
 * @brief Update the limit switches state with debouncing and stop the motor if a limit is reached.
 * @note This function should be called in the loop function.
 *
 * @param stopOnLimit Flag indicating whether to stop the motor when a limit is reached.
 */
void Motor::updateLimitSwitches(bool stopOnLimit)
{
    // Check the positive limit switch if it is set
    if (_posLimitPin != GPIO_NUM_NC)
    {
        // Check if the limit state has changed and the debounce time has passed
        if (_lastPosLimitState != _posLimitState && millis() - _lastPosLimitTriggerTime > _debounceTime)
        {
            // Update the last state
            _lastPosLimitState = _posLimitState;
            // Set the limit reached flag
            posLimitReached = _posLimitState;
            // Update the last trigger time
            _lastPosLimitTriggerTime = millis();
            // Stop the motor if the limit is reached and stopOnLimit is true
            if (posLimitReached && stopOnLimit)
                stopImmediate();
        }
    }

    // Check the negative limit switch if it is set
    if (_negLimitPin != GPIO_NUM_NC)
    {
        // Check if the limit state has changed and the debounce time has passed
        if (_lastNegLimitState != _negLimitState && millis() - _lastNegLimitTriggerTime > _debounceTime)
        {
            // Update the last state
            _lastNegLimitState = _negLimitState;
            // Set the limit reached flag
            negLimitReached = _negLimitState;
            // Update the last trigger time
            _lastNegLimitTriggerTime = millis();
            // Stop the motor if the limit is reached
            if (negLimitReached)
                stopImmediate();
        }
    }
}

/**
 * @brief Handles the position limit reached event.
 *
 * This function is called when the position limit is reached. It updates the `_posLimitState`
 * variable based on the level of the `_posLimitPin` GPIO.
 *
 * @note This function is marked with the `IRAM_ATTR` attribute to ensure it is placed in the
 * IRAM (instruction RAM) section of the microcontroller's memory, which allows for faster
 * execution.
 */
void IRAM_ATTR Motor::_handlePosLimitReached()
{
    _posLimitState = !gpio_get_level(_posLimitPin);
}

/**
 * @brief Handles the negative limit reached event.
 *
 * This function is called when the negative limit is reached. It updates the `_negLimitState`
 * variable based on the level of the `_negLimitPin`.
 *
 * @note This function is marked with the `IRAM_ATTR` attribute to ensure it is placed in the
 * IRAM (instruction RAM) section of the microcontroller's memory, which allows for faster
 * execution.
 */
void IRAM_ATTR Motor::_handleNegLimitReached()
{
    _negLimitState = !gpio_get_level(_negLimitPin);
}

/**
 * @brief Stops the motor with desired braking mode.
 */
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

/**
 * @brief Stops the motor immediately (prior of other requests in a queue) with braking.
 */
void Motor::stopImmediate()
{
    setMotorPwm(_posMotorPin, _negMotorPin, PWM_ON, PWM_ON, true);
}

/**
 * @brief Sets the speed of the motor.
 *
 * @param speed The speed of the motor in the range of -255 to 255.
 */
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
