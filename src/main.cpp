#include <WiFi.h>
#include <Wire.h>
#include "driver/temp_sensor.h"
#include "GyverMotor2.h"

#include "constants.h"
#include "data_structures.h"
#include "esp_now_manager.h"
#include "wifi_ota_manager.h"

// Create a variable to store the received data
controller_data_struct receivedData;

// Create a variable to store the data that will be sent to the Controller
excavator_data_struct dataToSend;

GMotor2<DRIVER2WIRE_PWM> boomMotor(BOOM_MOTOR_POS, BOOM_MOTOR_NEG);
// TODO: only 8 PWM pins available on ESP32
// GMotor2<DRIVER2WIRE_PWM> bucketMotor(BUCKET_MOTOR_POS, BUCKET_MOTOR_NEG);
// GMotor2<DRIVER2WIRE_PWM> stickMotor(STICK_MOTOR_POS, STICK_MOTOR_NEG);
GMotor2<DRIVER2WIRE_PWM> swingMotor(SWING_MOTOR_POS, SWING_MOTOR_NEG);
GMotor2<DRIVER2WIRE_PWM> leftTravelMotor(LEFT_TRAVEL_MOTOR_POS, LEFT_TRAVEL_MOTOR_NEG);
GMotor2<DRIVER2WIRE_PWM> rightTravelMotor(RIGHT_TRAVEL_MOTOR_POS, RIGHT_TRAVEL_MOTOR_NEG);

int16_t cpuTemp = -999;
uint32_t lastTempReadTime = 0;

void initTempSensor()
{
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor.dac_offset = TSENS_DAC_L2;
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

int16_t getCpuTemp()
{
    static float cpuTempF = 0.0;
    temp_sensor_read_celsius(&cpuTempF);
    // Multiply temperature by 100 to keep 2 decimal places
    return (int16_t)(cpuTempF * 100.0);
}

// Callback when data from Controller received
void onDataFromController(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("Received from Controller: Boom: %3d | Bucket: %3d | Stick: %3d | Swing: %3d | Track Left: %3d | Track Right: %3d | Battery: %3d\n",
                  receivedData.leverPositions[0], receivedData.leverPositions[1], receivedData.leverPositions[2],
                  receivedData.leverPositions[3], receivedData.leverPositions[4], receivedData.leverPositions[5],
                  receivedData.battery);

    boomMotor.setSpeed(receivedData.leverPositions[0]);
    // bucketMotor.setSpeed(receivedData.leverPositions[1]);
    // stickMotor.setSpeed(receivedData.leverPositions[2]);
    swingMotor.setSpeed(receivedData.leverPositions[3]);
    leftTravelMotor.setSpeed(receivedData.leverPositions[4]);
    rightTravelMotor.setSpeed(receivedData.leverPositions[5]);

    // Send the dataToSend to the Controller
    dataToSend.uptime = millis() / 1000;
    dataToSend.battery = 100;
    dataToSend.cpuTemp = cpuTemp;
    sendDataToController(dataToSend);
}

void setup()
{
    // Setup pins
    // pinMode(BOOM_LOW_LIMIT, INPUT_PULLUP);
    // pinMode(BOOM_HIGH_LIMIT, INPUT_PULLUP);
    pinMode(BUCKET_ROLL_IN_LIMIT, INPUT_PULLUP);
    pinMode(BUCKET_ROLL_OUT_LIMIT, INPUT_PULLUP);
    pinMode(STICK_ROLL_IN_LIMIT, INPUT_PULLUP);
    pinMode(STICK_ROLL_OUT_LIMIT, INPUT_PULLUP);
    pinMode(SWING_CENTER_SWITCH, INPUT_PULLUP);

    // pinMode(RGB_LED_BUILTIN, OUTPUT);
    pinMode(BOOM_LIGHTS, OUTPUT);

    digitalWrite(BOOM_LIGHTS, HIGH);

    leftTravelMotor.reverse(true);
    rightTravelMotor.reverse(true);

    // Init Temp Sensor
    initTempSensor();

    // Init Serial Monitor
    Serial.begin(115200);

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

    // Read CPU temperature every
    if (millis() - lastTempReadTime > TEMP_READ_INTERVAL)
    {
        lastTempReadTime = millis();
        cpuTemp = getCpuTemp();
    }
}