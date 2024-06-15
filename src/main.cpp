#include <Arduino.h>
#include <WiFi.h>


void setup() {
  Serial.begin(9600);

  // Print MAC address
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}

void loop() {

}
