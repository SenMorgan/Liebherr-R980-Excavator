#include <WiFi.h>

#include "constants.h"
#include "data_structures.h"
#include "esp_now_manager.h"
#include "motor.h"
#include "pwm_controller.h"
#include "wifi_ota_manager.h"

// Create Motor objects for each motor
Motor boomMotor(BOOM_MOTOR_POS, BOOM_MOTOR_NEG);
Motor bucketMotor(BUCKET_MOTOR_POS, BUCKET_MOTOR_NEG);
Motor stickMotor(STICK_MOTOR_POS, STICK_MOTOR_NEG, true, true);
Motor swingMotor(SWING_MOTOR_POS, SWING_MOTOR_NEG, false, true);
Motor leftTravelMotor(LEFT_TRAVEL_MOTOR_POS, LEFT_TRAVEL_MOTOR_NEG, false);
Motor rightTravelMotor(RIGHT_TRAVEL_MOTOR_POS, RIGHT_TRAVEL_MOTOR_NEG, false);

// Create a variable to store the received data
controller_data_struct receivedData;

// Create a variable to store the data that will be sent to the Controller
excavator_data_struct dataToSend;

// Callback when data from Controller received
void onDataFromController(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("Received from Controller: Boom: %3d | Bucket: %3d | Stick: %3d | Swing: %3d | "
                  "Track Left: %3d | Track Right: %3d | Lights: %d | Center Swing: %d | Battery: %3d\n",
                  receivedData.leverPositions[0], receivedData.leverPositions[1], receivedData.leverPositions[2],
                  receivedData.leverPositions[3], receivedData.leverPositions[4], receivedData.leverPositions[5],
                  receivedData.buttonsStates[0], receivedData.buttonsStates[1], receivedData.battery);

    // Control motors based on received data
    boomMotor.setSpeed(receivedData.leverPositions[0]);
    bucketMotor.setSpeed(receivedData.leverPositions[1]);
    stickMotor.setSpeed(receivedData.leverPositions[2]);
    swingMotor.setSpeed(receivedData.leverPositions[3]);
    leftTravelMotor.setSpeed(receivedData.leverPositions[4]);
    rightTravelMotor.setSpeed(receivedData.leverPositions[5]);

    // Control lights
    if (receivedData.buttonsStates[0])
    {
        digitalWrite(BOOM_LIGHTS, HIGH);
        setPinPWM(CABIN_FRONT_LIGHTS, PWM_ON);
        setPinPWM(CABIN_BACK_LIGHTS, PWM_ON);
        setPinPWM(LEFT_LIGHT, PWM_ON);
        setPinPWM(RIGHT_LIGHT, PWM_ON);
    }
    else
    {
        digitalWrite(BOOM_LIGHTS, LOW);
        setPinPWM(CABIN_FRONT_LIGHTS, PWM_OFF);
        setPinPWM(CABIN_BACK_LIGHTS, PWM_OFF);
        setPinPWM(LEFT_LIGHT, PWM_OFF);
        setPinPWM(RIGHT_LIGHT, PWM_OFF);
    }
}

void setup()
{
    // Setup pins
    pinMode(SWING_CENTER_SWITCH, INPUT_PULLUP);

    // pinMode(RGB_LED_BUILTIN, OUTPUT);
    pinMode(BOOM_LIGHTS, OUTPUT);
    digitalWrite(BOOM_LIGHTS, HIGH);

    // Init Serial Monitor
    Serial.begin(115200);

    // Setup PWM
    pwmTaskInit();

    // Setup motors
    boomMotor.setupLimitSwitches(BOOM_LOW_LIMIT, BOOM_HIGH_LIMIT);
    bucketMotor.setupLimitSwitches(BUCKET_ROLL_IN_LIMIT, BUCKET_ROLL_OUT_LIMIT);
    stickMotor.setupLimitSwitches(STICK_ROLL_IN_LIMIT, STICK_ROLL_OUT_LIMIT);

    // Init Wi-Fi and OTA
    setupWiFi();
    setupOTA();
    // Init ESP-NOW
    initEspNow();

    // Register callback for data received from Controller
    registerDataRecvCallback(onDataFromController);

    // Finish initialization by logging message
    Serial.printf("\n%s [%s] initialized\n", HOSTNAME, WiFi.macAddress().c_str());
}

void loop()
{
    handleOTA();

    static bool lastPosLimitState = false;
    static bool lastNegLimitState = false;

    if (bucketMotor.posLimitReached != lastPosLimitState)
    {
        lastPosLimitState = bucketMotor.posLimitReached;
        Serial.println("Positive limit reached: " + String(lastPosLimitState));
    }
    if (bucketMotor.negLimitReached != lastNegLimitState)
    {
        lastNegLimitState = bucketMotor.negLimitReached;
        Serial.println("Negative limit reached: " + String(lastNegLimitState));
    }
}