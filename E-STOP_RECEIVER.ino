#include <WiFi.h>
#include <esp_now.h>

int brakePin = 5;  


void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (incomingData[0] == 0x01) {
    Serial.println("Emergency Stop Command Received - Activating Brakes");
    digitalWrite(brakePin, LOW); 
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(brakePin, OUTPUT);
  digitalWrite(brakePin, HIGH); 


  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }


  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
  delay(100); 
}
