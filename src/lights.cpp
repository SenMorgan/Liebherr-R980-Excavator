/**
 * @file lights.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-07-07
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "lights.h"
#include <hal/ledc_types.h>

#include "constants.h"
#include "pwm_controller.h"

// Light parameters
#define NUM_LIGHTS                 5     // Total number of lights
#define LIGHTS_CHANGE_RATE         100   // Rate at which the brightness of the lights changes (PWM units per cycle)
#define LIGHTS_GPIO_PWM_FREQUENCY  12000 // PWM frequency for GPIO-controlled lights
#define LIGHTS_GPIO_PWM_RESOLUTION 10    // PWM resolution for GPIO-controlled lights

// Task parameters
#define LIGHTS_TASK_FREQUENCY_HZ (50U)
#define LIGHTS_TASK_STACK_SIZE   (2 * 1024U)
#define LIGHTS_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)
#define LIGHTS_TASK_CORE         1 // Core 0 is used by the WiFi

// Helper macros
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

// Light objects
Light boomLights = {DIRECT_GPIO, .gpio = {BOOM_LIGHTS, LEDC_CHANNEL_0}};
Light cabinFrontLights = {EXPANDER, .expPin = CABIN_FRONT_LIGHTS};
Light cabinBackLights = {EXPANDER, .expPin = CABIN_BACK_LIGHTS};
Light leftLight = {EXPANDER, .expPin = LEFT_LIGHT};
Light rightLight = {EXPANDER, .expPin = RIGHT_LIGHT};

// Global array of all lights
Light lights[NUM_LIGHTS] = {boomLights, cabinFrontLights, cabinBackLights, leftLight, rightLight};

/**
 * @brief Set the brightness of a light depending on the control method.
 *
 * @param light Pointer to the Light object representing the light.
 * @param value The brightness value to set.
 */
void _setLightBrightness(Light *light, uint16_t value)
{
    if (light->controlMethod == DIRECT_GPIO)
    {
        ledcWrite(light->gpio.ledc_channel, value);
    }
    else if (light->controlMethod == EXPANDER)
    {
        setPinPWM(light->expPin, value);
    }
}

/**
 * @brief Update the brightness of a light.
 * This function should be called periodically to update the brightness of the light.
 *
 * @param light Pointer to the Light object representing the light.
 */
void _updateLight(Light *light)
{
    // Increase or decrease brightness based on targetPWM
    if (light->currentPWM < light->targetPWM)
    {
        light->currentPWM = min(light->currentPWM + LIGHTS_CHANGE_RATE, PWM_ON);
        _setLightBrightness(light, light->currentPWM);
    }
    else if (light->currentPWM > light->targetPWM)
    {
        light->currentPWM = max(light->currentPWM - LIGHTS_CHANGE_RATE, PWM_OFF);
        _setLightBrightness(light, light->currentPWM);
    }
    // If currentPWM == targetPWM, no action needed
}

/**
 * @brief Task function for controlling the lights.
 *
 * This task initializes the lights and continuously updates their states based on the target PWM values.
 * The lights are updated periodically with a frequency defined by LIGHTS_TASK_FREQUENCY_HZ.
 *
 * @param pvParameters A pointer to task parameters (not used in this function).
 */
void lightsTask(void *pvParameters)
{
    const TickType_t xFrequency = pdMS_TO_TICKS(1000 / LIGHTS_TASK_FREQUENCY_HZ);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    // Initialize lights
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        if (lights[i].controlMethod == DIRECT_GPIO)
        {
            ledcAttachPin(lights[i].gpio.pin, lights[i].gpio.ledc_channel);
            ledcSetup(lights[i].gpio.ledc_channel, LIGHTS_GPIO_PWM_FREQUENCY, LIGHTS_GPIO_PWM_RESOLUTION);
        }
    }

    // Power on boom lights by default
    boomLights.targetPWM = PWM_ON;

    Serial.println("lightsTask started");

    // Main task loop
    for (;;)
    {
        // Update lights (need to call every light manually, cause automatic iterating
        // over the lights array does not work by unknown reason)
        _updateLight(&boomLights);
        _updateLight(&cabinFrontLights);
        _updateLight(&cabinBackLights);
        _updateLight(&leftLight);
        _updateLight(&rightLight);

        // Wait for the next cycle.
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * @brief Initializes the lights task.
 *
 * This function creates a new task called "lightsTask" and assigns it to a specific core.
 *
 * @note This function should be called once during the setup phase of the program.
 */
void lightsTaskInit(void)
{
    if (pdPASS != xTaskCreatePinnedToCore(lightsTask,
                                          "lightsTask",
                                          LIGHTS_TASK_STACK_SIZE,
                                          NULL,
                                          LIGHTS_TASK_PRIORITY,
                                          NULL,
                                          LIGHTS_TASK_CORE))
    {
        Serial.println("Failed to create lightsTask");
    }
}