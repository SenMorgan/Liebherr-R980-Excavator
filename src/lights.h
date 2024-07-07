/**
 * @file lights.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-07-07
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef LIGHTS_H
#define LIGHTS_H

#include <Arduino.h>

enum LightControlMethod
{
    DIRECT_GPIO,
    EXPANDER
};

struct Light
{
    LightControlMethod controlMethod; // Specifies the type of light
    union
    {
        struct
        {
            gpio_num_t pin;       // GPIO pin number
            uint8_t ledc_channel; // LEDC channel for PWM control
        } gpio;
        uint8_t expPin; // Pin number on the expander
    };
    uint16_t targetPWM;  // Desired PWM value
    uint16_t currentPWM; // Current PWM value
};

extern Light boomLights;
extern Light cabinFrontLights;
extern Light cabinBackLights;
extern Light leftLight;
extern Light rightLight;

void lightsTaskInit();

#endif // LIGHTS_H