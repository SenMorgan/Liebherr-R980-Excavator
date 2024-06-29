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

// Limit switches
// #define BOOM_LOW_LIMIT
// #define BOOM_HIGH_LIMIT
#define BUCKET_ROLL_IN_LIMIT  GPIO_NUM_14    // PWM at startup - change with boom lights?
#define BUCKET_ROLL_OUT_LIMIT GPIO_NUM_13
#define STICK_ROLL_IN_LIMIT   GPIO_NUM_11
#define STICK_ROLL_OUT_LIMIT  GPIO_NUM_10
#define SWING_CENTER_SWITCH   GPIO_NUM_19

// Motor pins
#define BOOM_MOTOR_POS         GPIO_NUM_35
#define BOOM_MOTOR_NEG         GPIO_NUM_36
#define BUCKET_MOTOR_POS       GPIO_NUM_39
#define BUCKET_MOTOR_NEG       GPIO_NUM_40
#define STICK_MOTOR_POS        GPIO_NUM_38
#define STICK_MOTOR_NEG        GPIO_NUM_37
#define SWING_MOTOR_POS        GPIO_NUM_47
#define SWING_MOTOR_NEG        GPIO_NUM_48
#define LEFT_TRAVEL_MOTOR_POS  GPIO_NUM_2
#define LEFT_TRAVEL_MOTOR_NEG  GPIO_NUM_1
#define RIGHT_TRAVEL_MOTOR_POS GPIO_NUM_42
#define RIGHT_TRAVEL_MOTOR_NEG GPIO_NUM_41

#endif // _CONSTANTS_H