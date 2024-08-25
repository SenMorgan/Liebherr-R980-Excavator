/**
 * @file power_manager.cpp
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-08-20
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#include "constants.h"
#include <data_structures.h>
#include "power_manager.h"

// Interval between battery voltage readings
#define READ_BATTERY_INTERVAL_MS 1000

// Formula to calculate the battery voltage
#define CALCULATE_BATT_MV(mv) ((mv) * 0.925 + 1056)

// The number of readings to average
#define NUM_READINGS 5

// Task parameters
#define POWER_MANAGER_TASK_FREQUENCY_HZ (50U)
#define POWER_MANAGER_TASK_STACK_SIZE   (2 * 1024U)
#define POWER_MANAGER_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)

extern excavator_data_struct dataToSend;

uint32_t lastBatteryReadTime = 0;

uint16_t getAveragedBattVoltage()
{
    uint32_t total = 0;

    // Take multiple readings
    for (int i = 0; i < NUM_READINGS; ++i)
    {
        total += analogRead(BATTERY_VOLTAGE_PIN);

        // Short delay between readings
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Calculate the average reading
    uint16_t avgReadMv = total / NUM_READINGS;

    // Calculate the battery voltage using formula
    return CALCULATE_BATT_MV(avgReadMv);
}

/**
 * @brief Task function for managing the power of the machine.
 *
 * @param pvParameters A pointer to task parameters (not used in this function).
 */
void powerManagerTask(void *pvParameters)
{
    const TickType_t xFrequency = pdMS_TO_TICKS(READ_BATTERY_INTERVAL_MS);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    analogSetPinAttenuation(BATTERY_VOLTAGE_PIN, ADC_11db);

    Serial.println("powerManagerTask started");

    // Main task loop
    for (;;)
    {
        // Read the battery voltage
        dataToSend.battery = getAveragedBattVoltage();

        // Wait for the next cycle.
        xTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * @brief Initializes the power manager task.
 *
 * @note This function should be called once during the setup phase of the program.
 */
void powerManagerTaskInit(void)
{
    if (pdPASS != xTaskCreate(powerManagerTask,
                              "powerManagerTask",
                              POWER_MANAGER_TASK_STACK_SIZE,
                              NULL,
                              POWER_MANAGER_TASK_PRIORITY,
                              NULL))
    {
        Serial.println("Failed to create powerManagerTask");
    }
}
