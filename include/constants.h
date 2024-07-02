/**
 * @file constants.h
 * @author SenMorgan https://github.com/SenMorgan
 * @date 2024-06-16
 *
 * @copyright Copyright (c) 2024 Sen Morgan
 *
 */

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// Period between CPU temperature readings
#define TEMP_READ_INTERVAL 1000

// Number of levers
#define LEVERS_COUNT 6

// Number of buttons
#define BUTTONS_COUNT 2

// IO pins
// #define RGB_LED_BUILTIN GPIO_NUM_48

// Lights
#define BOOM_LIGHTS GPIO_NUM_12

// Lights connected to PCA9685
#define LEFT_LIGHT         0
#define RIGHT_LIGHT        1
#define CABIN_FRONT_LIGHTS 2
#define CABIN_BACK_LIGHTS  3

// Limit switches
// #define BOOM_LOW_LIMIT
// #define BOOM_HIGH_LIMIT
#define BUCKET_ROLL_IN_LIMIT  GPIO_NUM_14
#define BUCKET_ROLL_OUT_LIMIT GPIO_NUM_13
#define STICK_ROLL_IN_LIMIT   GPIO_NUM_11
#define STICK_ROLL_OUT_LIMIT  GPIO_NUM_10
#define SWING_CENTER_SWITCH   GPIO_NUM_19

// Motor driver pins connected to PCA9685
#define BOOM_MOTOR_POS         14
#define BOOM_MOTOR_NEG         15
#define BUCKET_MOTOR_POS       11
#define BUCKET_MOTOR_NEG       10
#define STICK_MOTOR_POS        9
#define STICK_MOTOR_NEG        8
#define SWING_MOTOR_POS        12
#define SWING_MOTOR_NEG        13
#define LEFT_TRAVEL_MOTOR_POS  6
#define LEFT_TRAVEL_MOTOR_NEG  7
#define RIGHT_TRAVEL_MOTOR_POS 5
#define RIGHT_TRAVEL_MOTOR_NEG 4

#endif // _CONSTANTS_H