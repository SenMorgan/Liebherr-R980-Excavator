/**
 * @file pwm_controller.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-07-02
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef PWM_CONTROLLER_H
#define PWM_CONTROLLER_H

#include <Arduino.h>

#define PWM_OFF    0
#define PWM_ON     255
#define PWM_NC_PIN 255

void pwmTaskInit(void);
void setPinPWM(uint8_t pin, uint8_t value);
void setMotorPwm(uint8_t posMotorPin, uint8_t negMotorPin, uint8_t posPinValue,
                 uint8_t negPinValue, bool immediate = false);

#endif // PWM_CONTROLLER_H