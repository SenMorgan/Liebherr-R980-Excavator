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
#define PWM_ON     1023
#define PWM_NC_PIN 255

void pwmTaskInit(void);
void setPinPWM(uint8_t pin, uint16_t value);
void setMotorPwm(uint8_t posMotorPin, uint8_t negMotorPin, uint16_t posPinValue,
                 uint16_t negPinValue, bool immediate = false);

#endif // PWM_CONTROLLER_H