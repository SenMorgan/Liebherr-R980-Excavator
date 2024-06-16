#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include "driver/temp_sensor.h"

#include "constants.h"
#include "data_structures.h"
#include "esp_now_manager.h"

// Create a variable to store the received data
controller_data_struct receivedData;

// Create a variable to store the data that will be sent to the Controller
excavator_data_struct dataToSend;

float cpuTemp = -999.0;

void initTempSensor()
{
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor.dac_offset = TSENS_DAC_L2;
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

// Callback when data from Controller received
void onDataFromController(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.printf("\nReceived from Controller:\nBoom: %d\nDipper: %d\nBucket: %d\nSwing: %d\nTrack Left: %d\nTrack Right: %d\n",
                  receivedData.boom, receivedData.dipper, receivedData.bucket, receivedData.swing, receivedData.trackLeft, receivedData.trackRight);

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

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    initEspNow();
    registerDataRecvCallback(onDataFromController);
}

void loop()
{
    temp_sensor_read_celsius(&cpuTemp);
    // Serial.printf("CPU Temp: %.2f °C\n", cpuTemp);

    // Wait some time before next iteration
    delay(1000);
}
