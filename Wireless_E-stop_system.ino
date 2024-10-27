#include <WiFi.h>
#include <esp_now.h>

// Set mode: "TRANSMITTER" for E-stop button, "RECEIVER" for relay control
#define MODE "TRANSMITTER" // Change to "RECEIVER" on the receiver device

// Replace this with the MAC address of the receiver ESP32 (for the transmitter code)
uint8_t receiverAddress[] = {0x24, 0x0A, 0xC4, 0xD8, 0xE4, 0xB8}; // Example MAC address

// E-stop status structure
typedef struct struct_message {
  bool estop;
} struct_message;

struct_message estopData;

#if MODE == "TRANSMITTER"
const int buttonPin = 0; // E-stop button connected to GPIO 0
#else
const int relayPin = 2;  // Relay connected to GPIO 2 (for receiver)
#endif

// Callback function for successful transmission (transmitter only)
void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.print("Delivery Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Callback function to handle incoming data (receiver only)
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&estopData, incomingData, sizeof(estopData));

  if (estopData.estop) {
    digitalWrite(relayPin, HIGH); // Activate relay (stop machinery)
    Serial.println("E-Stop activated!");
  } else {
    digitalWrite(relayPin, LOW); // Deactivate relay (run machinery)
    Serial.println("System running normally.");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize Wi-Fi in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  #if MODE == "TRANSMITTER"
    // Configure transmitter setup
    pinMode(buttonPin, INPUT);
    
    // Register peer for communication
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
    }

    // Register callback for send status
    esp_now_register_send_cb(onSent);

  #else
    // Configure receiver setup
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW); // Initialize relay as OFF
    
    // Register callback for received data
    esp_now_register_recv_cb(onDataRecv);
  #endif
}

void loop() {
  #if MODE == "TRANSMITTER"
    // Transmitter: Read button status and send data
    estopData.estop = digitalRead(buttonPin); // Read button status (HIGH or LOW)

    // Send E-stop data to receiver
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &estopData, sizeof(estopData));
    if (result == ESP_OK) {
      Serial.println("Sent successfully");
    } else {
      Serial.println("Error sending data");
    }

    delay(100); // Delay to prevent spamming

  #else
    // Receiver: Nothing to do in loop, only listens for incoming data
    delay(100);
  #endif
}
