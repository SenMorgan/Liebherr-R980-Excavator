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

// IO pins
// #define RGB_LED_BUILTIN 48

// Lights
#define BOOM_LIGHTS 12

// Limit switches
// #define BOOM_LOW_LIMIT
// #define BOOM_HIGH_LIMIT
#define BUCKET_ROLL_IN_LIMIT  14    // PWM at startup - change with boom lights?
#define BUCKET_ROLL_OUT_LIMIT 13
#define STICK_ROLL_IN_LIMIT   11
#define STICK_ROLL_OUT_LIMIT  10
#define SWING_CENTER_SWITCH   19

// Motor pins
#define BOOM_MOTOR_POS         35
#define BOOM_MOTOR_NEG         36
#define BUCKET_MOTOR_POS       39
#define BUCKET_MOTOR_NEG       40
#define STICK_MOTOR_POS        38
#define STICK_MOTOR_NEG        37
#define SWING_MOTOR_POS        47
#define SWING_MOTOR_NEG        48
#define LEFT_TRAVEL_MOTOR_POS  2
#define LEFT_TRAVEL_MOTOR_NEG  1
#define RIGHT_TRAVEL_MOTOR_POS 42
#define RIGHT_TRAVEL_MOTOR_NEG 41

#endif // _CONSTANTS_H