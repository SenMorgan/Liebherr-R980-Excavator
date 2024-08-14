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

enum LightMode
{
    OFF,
    FRONT_LIGHTS,
    FRONT_BACK_LIGHTS,
    FRONT_BACK_SIDES_LIGHTS,
    ALL_LIGHTS,
    ALL_LIGHTS_WITH_BLINKING
};

enum LightControlMethod
{
    DIRECT_GPIO,
    EXPANDER
};

enum
{
    BOOM_LIGHTS,
    ROOF_FRONT_LIGHTS,
    ROOF_BACK_LIGHTS,
    LEFT_HEADLIGHT,
    RIGHT_HEADLIGHT,
    // Total number of lights
    NUM_LIGHTS
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

void lightsTaskInit();
void nextLightMode();
void beaconLightChangeMode();

#endif // LIGHTS_H