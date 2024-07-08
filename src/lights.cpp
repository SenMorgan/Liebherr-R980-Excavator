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

// Global array of all lights
Light lights[NUM_LIGHTS] = {
    [BOOM_LIGHTS] = {.controlMethod = DIRECT_GPIO, .gpio = {BOOM_LIGHTS_PIN, LEDC_CHANNEL_0}},
    [ROOF_FRONT_LIGHTS] = {.controlMethod = EXPANDER, .expPin = ROOF_FRONT_LIGHTS_PIN},
    [ROOF_BACK_LIGHTS] = {.controlMethod = EXPANDER, .expPin = ROOF_BACK_LIGHTS_PIN},
    [LEFT_HEADLIGHT] = {.controlMethod = EXPANDER, .expPin = LEFT_HEADLIGHT_PIN},
    [RIGHT_HEADLIGHT] = {.controlMethod = EXPANDER, .expPin = RIGHT_HEADLIGHT_PIN}};

LightMode currentLightMode = ALL_LIGHTS_WITH_BLINKING;

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
 * @brief Updates the lights states based on the current light mode.
 */
void _updateLightsMode()
{
#define BLINK_INTERVAL_MS 500
    static uint32_t lastBlinkTime = 0;
    static bool blinkState = false;

    // Update lights based on the current mode
    switch (currentLightMode)
    {
        case OFF:
            // Turn off all lights
            for (int i = 0; i < NUM_LIGHTS; ++i)
                lights[i].targetPWM = PWM_OFF;
            break;
        case FRONT_LIGHTS:
            // Turn on only front lights
            lights[ROOF_FRONT_LIGHTS].targetPWM = PWM_ON;
            break;
        case FRONT_BACK_LIGHTS:
            // Turn on front and back lights
            lights[ROOF_FRONT_LIGHTS].targetPWM = PWM_ON;
            lights[ROOF_BACK_LIGHTS].targetPWM = PWM_ON;
            break;
        case FRONT_BACK_SIDES_LIGHTS:
            // Turn on front, back, left, and right lights
            lights[ROOF_FRONT_LIGHTS].targetPWM = PWM_ON;
            lights[ROOF_BACK_LIGHTS].targetPWM = PWM_ON;
            lights[LEFT_HEADLIGHT].targetPWM = PWM_ON;
            lights[RIGHT_HEADLIGHT].targetPWM = PWM_ON;
            break;
        case ALL_LIGHTS:
            // Turn on all lights
            for (int i = 0; i < NUM_LIGHTS; ++i)
                lights[i].targetPWM = PWM_ON;
            break;
        case ALL_LIGHTS_WITH_BLINKING:
            // All lights on, but left and right lights blink
            lights[BOOM_LIGHTS].targetPWM = PWM_ON;
            lights[ROOF_FRONT_LIGHTS].targetPWM = PWM_ON;
            lights[ROOF_BACK_LIGHTS].targetPWM = PWM_ON;

            if (millis() - lastBlinkTime >= BLINK_INTERVAL_MS)
            {
                lastBlinkTime = millis();
                blinkState = !blinkState;

                if (blinkState)
                {
                    lights[LEFT_HEADLIGHT].targetPWM = PWM_ON;
                    lights[RIGHT_HEADLIGHT].targetPWM = PWM_OFF;
                }
                else
                {
                    lights[LEFT_HEADLIGHT].targetPWM = PWM_OFF;
                    lights[RIGHT_HEADLIGHT].targetPWM = PWM_ON;
                }
            }
            break;
    }
}

/**
 * @brief Changes the light mode to the next mode in the sequence.
 *
 * This function updates the current light mode to the next mode in the sequence.
 * If the current mode is the last mode, it wraps around to the first mode.
 */
void nextLightMode()
{
    currentLightMode = static_cast<LightMode>((currentLightMode + 1) % (ALL_LIGHTS_WITH_BLINKING + 1));
    Serial.printf("Light mode changed to %d\n", currentLightMode);
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
    lights[BOOM_LIGHTS].targetPWM = PWM_ON;

    Serial.println("lightsTask started");

    // Main task loop
    for (;;)
    {
        // Iterate over all lights and update their brightness
        for (int i = 0; i < NUM_LIGHTS; ++i)
            _updateLight(&lights[i]);

        _updateLightsMode();

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