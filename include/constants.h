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
#define BUTTONS_COUNT 3

// IO pins
#define BATTERY_VOLTAGE_PIN    GPIO_NUM_36
#define MOTOR_DRIVER_FAULT_PIN GPIO_NUM_16
#define MOTOR_DRIVER_SLEEP_PIN GPIO_NUM_17
#define EXTRA_IO_PIN           GPIO_NUM_14

// Lights connected directly to ESP32
#define BOOM_LIGHTS_PIN  GPIO_NUM_26
#define BEACON_LIGHT_PIN GPIO_NUM_13
#define REAR_LIGHTS_PIN  GPIO_NUM_5

// Lights connected via expander
#define LEFT_HEADLIGHT_PIN    0
#define RIGHT_HEADLIGHT_PIN   1
#define ROOF_BACK_LIGHTS_PIN  2
#define ROOF_FRONT_LIGHTS_PIN 3

// Limit switches
#define BOOM_LOW_LIMIT_PIN        GPIO_NUM_19
#define BOOM_HIGH_LIMIT_PIN       GPIO_NUM_23
#define BUCKET_ROLL_IN_LIMIT_PIN  GPIO_NUM_33
#define BUCKET_ROLL_OUT_LIMIT_PIN GPIO_NUM_25
#define STICK_ROLL_IN_LIMIT_PIN   GPIO_NUM_27
#define STICK_ROLL_OUT_LIMIT_PIN  GPIO_NUM_12
#define SWING_CENTER_SWITCH_PIN   GPIO_NUM_32

// Motor driver pins connected via expander
#define BOOM_MOTOR_POS_PIN         14
#define BOOM_MOTOR_NEG_PIN         15
#define BUCKET_MOTOR_POS_PIN       10
#define BUCKET_MOTOR_NEG_PIN       11
#define STICK_MOTOR_POS_PIN        9
#define STICK_MOTOR_NEG_PIN        8
#define SWING_MOTOR_POS_PIN        12
#define SWING_MOTOR_NEG_PIN        13
#define LEFT_TRAVEL_MOTOR_POS_PIN  6
#define LEFT_TRAVEL_MOTOR_NEG_PIN  7
#define RIGHT_TRAVEL_MOTOR_POS_PIN 5
#define RIGHT_TRAVEL_MOTOR_NEG_PIN 4

#endif // _CONSTANTS_H