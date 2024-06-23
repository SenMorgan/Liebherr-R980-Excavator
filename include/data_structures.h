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

// The structure type of the data that will be sent over ESP-NOW from the Controller to the Excavator
typedef struct controller_data_struct
{
    int16_t boomPos;        // Position of the boom lever
    int16_t bucketPos;      // Position of the bucket lever
    int16_t stickPos;       // Position of the stick lever
    int16_t swingPos;       // Position of the swing lever
    int16_t travelLeftPos;  // Position of the left travel lever
    int16_t travelRightPos; // Position of the right travel lever
    uint16_t battery;       // Controller battery voltage
} controller_data_struct;

// The structure type of the data that will be sent over ESP-NOW from the Excavator to the Controller
typedef struct excavator_data_struct
{
    uint16_t uptime;  // Uptime of the Excavator in seconds
    uint16_t battery; // Battery level of the Excavator
    int16_t cpuTemp;  // CPU temperature of the Excavator
} excavator_data_struct;

#endif // DATA_STRUCTURES_H