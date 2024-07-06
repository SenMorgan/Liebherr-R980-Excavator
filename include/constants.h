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

// Number of levers
#define LEVERS_COUNT 6

// Number of buttons
#define BUTTONS_COUNT 2

// IO pins
#define MOTOR_DRIVER_FAULT GPIO_NUM_16
#define MOTOR_DRIVER_SLEEP GPIO_NUM_17

// Lights
#define BOOM_LIGHTS GPIO_NUM_26

// Lights connected to PCA9685
#define LEFT_LIGHT         0
#define RIGHT_LIGHT        1
#define CABIN_FRONT_LIGHTS 2
#define CABIN_BACK_LIGHTS  3

// Limit switches
#define BOOM_LOW_LIMIT        GPIO_NUM_19
#define BOOM_HIGH_LIMIT       GPIO_NUM_23
#define BUCKET_ROLL_IN_LIMIT  GPIO_NUM_33
#define BUCKET_ROLL_OUT_LIMIT GPIO_NUM_25
#define STICK_ROLL_IN_LIMIT   GPIO_NUM_27
#define STICK_ROLL_OUT_LIMIT  GPIO_NUM_14
#define SWING_CENTER_SWITCH   GPIO_NUM_32

// Motor driver pins connected to PCA9685
#define BOOM_MOTOR_POS         14
#define BOOM_MOTOR_NEG         15
#define BUCKET_MOTOR_POS       10
#define BUCKET_MOTOR_NEG       11
#define STICK_MOTOR_POS        9
#define STICK_MOTOR_NEG        8
#define SWING_MOTOR_POS        12
#define SWING_MOTOR_NEG        13
#define LEFT_TRAVEL_MOTOR_POS  6
#define LEFT_TRAVEL_MOTOR_NEG  7
#define RIGHT_TRAVEL_MOTOR_POS 5
#define RIGHT_TRAVEL_MOTOR_NEG 4

#endif // _CONSTANTS_H