#include <WiFi.h>

#include "constants.h"
#include "data_structures.h"
#include "esp_now_manager.h"
#include "lights.h"
#include "motor.h"
#include "pwm_controller.h"
#include "wifi_ota_manager.h"

// Create Motor objects for each motor
Motor boomMotor(BOOM_MOTOR_POS_PIN, BOOM_MOTOR_NEG_PIN);
Motor bucketMotor(BUCKET_MOTOR_POS_PIN, BUCKET_MOTOR_NEG_PIN);
Motor stickMotor(STICK_MOTOR_POS_PIN, STICK_MOTOR_NEG_PIN, true, true);
Motor swingMotor(SWING_MOTOR_POS_PIN, SWING_MOTOR_NEG_PIN, false, true);
Motor leftTravelMotor(LEFT_TRAVEL_MOTOR_POS_PIN, LEFT_TRAVEL_MOTOR_NEG_PIN, true, true);
Motor rightTravelMotor(RIGHT_TRAVEL_MOTOR_POS_PIN, RIGHT_TRAVEL_MOTOR_NEG_PIN, true, true);

// Create a variable to store the received data
controller_data_struct receivedData;

// Create a variable to store the data that will be sent to the Controller
excavator_data_struct dataToSend;

// Callback when data from Controller received
void onDataFromController(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    static bool lastButtonsState[BUTTONS_COUNT] = {0};

    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("Received from Controller: Boom: %3d | Bucket: %3d | Stick: %3d | Swing: %3d | "
                  "Track Left: %3d | Track Right: %3d | Lights: %d | Center Swing: %d | Beacon: %d | Battery: %3d\n",
                  receivedData.leverPositions[0], receivedData.leverPositions[1], receivedData.leverPositions[2],
                  receivedData.leverPositions[3], receivedData.leverPositions[4], receivedData.leverPositions[5],
                  receivedData.buttonsStates[0], receivedData.buttonsStates[1], receivedData.buttonsStates[2],
                  receivedData.battery);

    // Control motors based on received data
    boomMotor.setSpeed(receivedData.leverPositions[0]);
    bucketMotor.setSpeed(receivedData.leverPositions[1]);
    stickMotor.setSpeed(receivedData.leverPositions[2]);
    swingMotor.setSpeed(receivedData.leverPositions[3]);
    leftTravelMotor.setSpeed(receivedData.leverPositions[4]);
    rightTravelMotor.setSpeed(receivedData.leverPositions[5]);

    // Change light mode
    if (lastButtonsState[0] != receivedData.buttonsStates[0])
    {
        lastButtonsState[0] = receivedData.buttonsStates[0];
        nextLightMode();
    }

    // Center swing position
    if (lastButtonsState[1] != receivedData.buttonsStates[1])
    {
        lastButtonsState[1] = receivedData.buttonsStates[1];
        Serial.println("Center Swing button state changed");
        // TODO: Implement swing center switch logic
    }

    // Change beacon light mode
    if (lastButtonsState[2] != receivedData.buttonsStates[2])
    {
        lastButtonsState[2] = receivedData.buttonsStates[2];
        beaconLightChangeMode();
    }
}

void setup()
{
    // Setup pins
    pinMode(SWING_CENTER_SWITCH_PIN, INPUT_PULLUP);

    // Init Serial Monitor
    Serial.begin(115200);

    // Setup PWM
    pwmTaskInit();

    // Setup lights
    lightsTaskInit();

    // Setup limit switches
    boomMotor.setupLimitSwitches(BOOM_LOW_LIMIT_PIN, BOOM_HIGH_LIMIT_PIN);
    bucketMotor.setupLimitSwitches(BUCKET_ROLL_IN_LIMIT_PIN, BUCKET_ROLL_OUT_LIMIT_PIN);
    stickMotor.setupLimitSwitches(STICK_ROLL_IN_LIMIT_PIN, STICK_ROLL_OUT_LIMIT_PIN);

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

    // Update limit switches
    boomMotor.updateLimitSwitches();
    bucketMotor.updateLimitSwitches();
    stickMotor.updateLimitSwitches();

    // Delay for some time to avoid high CPU usage
    delay(10);
}