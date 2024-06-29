/**
 * @file data_structures.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdint.h>

#include "constants.h"

// The structure type of the data that will be sent over ESP-NOW from the Controller to the Excavator
typedef struct controller_data_struct
{
    /*
     * The lever positions are stored in an array of 6 elements.
     * The order of the levers is as follows:
     * 0 - Boom
     * 1 - Bucket
     * 2 - Stick
     * 3 - Swing
     * 4 - Left Travel
     * 5 - Right Travel
     * The values are in the range of -255 to 255
     */
    int16_t leverPositions[LEVERS_COUNT];
    /*
     * The buttons states are stored in an array:
     * 0 - Lights button
     * 1 - Center Swing button
     */
    bool buttonsStates[BUTTONS_COUNT];
    // Controller battery voltage in millivolts
    uint16_t battery;
} controller_data_struct;

// The structure type of the data that will be sent over ESP-NOW from the Excavator to the Controller
typedef struct excavator_data_struct
{
    uint16_t uptime;  // Uptime of the Excavator in seconds
    uint16_t battery; // Battery level of the Excavator
    int16_t cpuTemp;  // CPU temperature of the Excavator
} excavator_data_struct;

#endif // DATA_STRUCTURES_H