/**
 * @file pwm_controller.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-07-02
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "pwm_controller.h"
#include <Adafruit_PWMServoDriver.h>

#define PWM_TASK_STACK_SIZE (2 * 1024U)
#define PWM_TASK_PRIORITY   (tskIDLE_PRIORITY + 2)
#define PWM_TASK_CORE       1 // Core 0 is used by the WiFi
#define QUEUE_LENGTH        50

/**
 * @brief Structure to hold PWM data. Values are in range 0-255
 */
struct PWMdata
{
    uint8_t pin1;
    uint8_t value1;
    uint8_t pin2;
    uint8_t value2;
};

QueueHandle_t pwmDataQueue;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void pwmTask(void *pvParameters)
{
    PWMdata pwmData;

    // Init PWM with maximum frequency
    pwm.begin();
    pwm.setPWMFreq(1600);

    // Reset all PWM channels
    for (uint8_t i = 0; i < 16; i++)
    {
        pwm.setPWM(i, 0, 0);
    }

    for (;;)
    {
        if (xQueueReceive(pwmDataQueue, &pwmData, portMAX_DELAY) == pdTRUE)
        {
            if (pwmData.pin1 != PWM_NC_PIN)
            {
                uint16_t pwmValue = map(pwmData.value1, 0, 255, 0, 4095);
                pwm.setPWM(pwmData.pin1, 0, pwmValue);
            }
            if (pwmData.pin2 != PWM_NC_PIN)
            {
                uint16_t pwmValue = map(pwmData.value2, 0, 255, 0, 4095);
                pwm.setPWM(pwmData.pin2, 0, pwmValue);
            }
        }
    }
}

void pwmTaskInit(void)
{
    // Create Queues
    pwmDataQueue = xQueueCreate(QUEUE_LENGTH, sizeof(PWMdata));
    configASSERT(pwmDataQueue);

    if (pdPASS != xTaskCreatePinnedToCore(pwmTask,
                                          "pwmTask",
                                          PWM_TASK_STACK_SIZE,
                                          NULL,
                                          PWM_TASK_PRIORITY,
                                          NULL,
                                          PWM_TASK_CORE))
    {
        Serial.println("Failed to create pwm_task");
    }
}

void setPinPWM(uint8_t pin, uint8_t value)
{
    PWMdata pwm;
    pwm.pin1 = pin;
    pwm.value1 = value;
    pwm.pin2 = PWM_NC_PIN;
    pwm.value2 = 0;

    // Send data to pwmTask
    if (xQueueSend(pwmDataQueue, &pwm, 0) != pdTRUE)
    {
        Serial.println("Failed to send data to pwmDataQueue");
    }
}

void setMotorPwm(uint8_t posMotorPin, uint8_t negMotorPin, uint8_t posPinValue, uint8_t negPinValue, bool immediate)
{
    PWMdata pwm;
    pwm.pin1 = posMotorPin;
    pwm.value1 = posPinValue;
    pwm.pin2 = negMotorPin;
    pwm.value2 = negPinValue;

    if (immediate)
    {
        // Remove any pending low priority messages
        xQueueReset(pwmDataQueue);
    }

    // Send data to pwmTask
    if (xQueueSend(pwmDataQueue, &pwm, 0) != pdTRUE)
    {
        Serial.println("Failed to send data to pwmDataQueue");
    }
}