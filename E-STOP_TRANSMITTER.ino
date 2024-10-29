#include <WiFi.h>
#include <esp_now.h>


uint8_t receiverAddress[] = {0x24, 0x0A, 0xC4, 0xD8, 0xE4, 0xB8}; 


typedef struct struct_message {
  bool estop;
} struct_message;

struct_message estopData;

void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.print("Delivery Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  // Initialize WiFi
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

  esp_now_register_send_cb(onSent);
}

void loop() {
  estopData.estop = digitalRead(0);

 
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &estopData, sizeof(estopData));

  if (result == ESP_OK) {
    Serial.println("Sent successfully");
  } else {
    Serial.println("Error sending data");
  }

  delay(100); 
}
