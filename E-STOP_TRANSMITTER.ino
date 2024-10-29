#include <WiFi.h>
#include <esp_now.h>

uint8_t receiverAddress[] = {0x24, 0x0A, 0xC4, 0xD8, 0xE4, 0xB8}; 
bool estop = false; 
int buttonPin = 4;  
int relayPin = 5;   

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); 

 
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }


  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  
  if (digitalRead(buttonPin) == HIGH) {
    estop = true;  
    digitalWrite(relayPin, LOW); 

 
    uint8_t stopCommand[] = {0x01};
    esp_now_send(receiverAddress, stopCommand, sizeof(stopCommand));
    Serial.println("Emergency Stop Command Sent!");

    delay(500); 
  }
}
