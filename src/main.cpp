#include <WiFi.h>
#include "driver/temp_sensor.h"
#include <SPI.h>
#include <Adafruit_PWMServoDriver.h>

#include "constants.h"
#include "data_structures.h"
#include "esp_now_manager.h"
#include "wifi_ota_manager.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Create a variable to store the received data
controller_data_struct receivedData;

// Create a variable to store the data that will be sent to the Controller
excavator_data_struct dataToSend;

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

void controlMotor(uint8_t posPin, uint8_t negPin, int16_t speed, bool breakMode = false, bool reverse = false)
{
    // Map -255 to 255 range to 0 to 4095 PWM range
    int pwmValue = map(abs(speed), 0, 255, 0, 4095);

    if (reverse)
    {
        // Swap the direction if reverse is true
        uint8_t tempPin = posPin;
        posPin = negPin;
        negPin = tempPin;
    }

    if (speed > 0)
    {
        // Forward direction
        pwm.setPWM(posPin, 0, pwmValue);
        pwm.setPWM(negPin, 0, 0);
    }
    else if (speed < 0)
    {
        // Reverse direction
        pwm.setPWM(posPin, 0, 0);
        pwm.setPWM(negPin, 0, pwmValue);
    }
    else
    {
        // If break mode is enabled, set both pins to HIGH
        if (breakMode)
        {
            pwm.setPWM(posPin, 0, 4095);
            pwm.setPWM(negPin, 0, 4095);
        }
        else
        {
            pwm.setPWM(posPin, 0, 0);
            pwm.setPWM(negPin, 0, 0);
        }
    }
}

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
    controlMotor(BOOM_MOTOR_POS, BOOM_MOTOR_NEG, receivedData.leverPositions[0], true);
    controlMotor(BUCKET_MOTOR_POS, BUCKET_MOTOR_NEG, receivedData.leverPositions[1], true);
    controlMotor(STICK_MOTOR_POS, STICK_MOTOR_NEG, receivedData.leverPositions[2], true);
    controlMotor(SWING_MOTOR_POS, SWING_MOTOR_NEG, receivedData.leverPositions[3], false, true);
    controlMotor(LEFT_TRAVEL_MOTOR_POS, LEFT_TRAVEL_MOTOR_NEG, receivedData.leverPositions[4]);
    controlMotor(RIGHT_TRAVEL_MOTOR_POS, RIGHT_TRAVEL_MOTOR_NEG, receivedData.leverPositions[5]);

    // Control lights
    if (receivedData.buttonsStates[0])
    {
        digitalWrite(BOOM_LIGHTS, HIGH);
        pwm.setPWM(CABIN_FRONT_LIGHTS, 0, 4095);
        pwm.setPWM(CABIN_BACK_LIGHTS, 0, 4095);
        pwm.setPWM(LEFT_LIGHT, 0, 4095);
        pwm.setPWM(RIGHT_LIGHT, 0, 4095);
    }
    else
    {
        digitalWrite(BOOM_LIGHTS, LOW);
        pwm.setPWM(CABIN_FRONT_LIGHTS, 0, 0);
        pwm.setPWM(CABIN_BACK_LIGHTS, 0, 0);
        pwm.setPWM(LEFT_LIGHT, 0, 0);
        pwm.setPWM(RIGHT_LIGHT, 0, 0);
    }
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

    // Init PWM with maximum frequency
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(1600);

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