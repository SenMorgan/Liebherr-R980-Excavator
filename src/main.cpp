#include <WiFi.h>
#include <Wire.h>
#include "driver/temp_sensor.h"

#include "constants.h"
#include "data_structures.h"
#include "esp_now_manager.h"
#include "wifi_ota_manager.h"

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

// Callback when data from Controller received
void onDataFromController(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("Received from Controller: Boom: %3d | Bucket: %3d | Stick: %3d | Swing: %3d | Track Left: %3d | Track Right: %3d | Battery: %3d\n",
                  receivedData.boomPos, receivedData.bucketPos, receivedData.stickPos, receivedData.swingPos,
                  receivedData.travelLeftPos, receivedData.travelRightPos, receivedData.battery);

    // Send the dataToSend to the Controller
    dataToSend.uptime = millis() / 1000;
    dataToSend.battery = 100;
    dataToSend.cpuTemp = cpuTemp;
    sendDataToController(dataToSend);
}

void setup()
{
    // Setup pins
    pinMode(RGB_LED_BUILTIN, OUTPUT);

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
    Serial.println(HOSTNAME + String(" initialized"));
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