#include <WiFi.h>
#include <esp_now.h>

int relayPin = 2;
typedef struct struct_message {
  bool estop;
} struct_message;

struct_message estopData;

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&estopData, incomingData, sizeof(estopData));
  
  if (estopData.estop) {
    digitalWrite(relayPin, HIGH);
    Serial.println("E-Stop activated!");
  } else {
    digitalWrite(relayPin, LOW); 
    Serial.println("System running normally.");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); 

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
