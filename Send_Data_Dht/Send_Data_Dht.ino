// Include required libraries
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>
 
// Define DHT22 parameters
#define DHTPin 4
#define DHTType DHT11
#define BOARD_ID 2
 
// Create DHT Object
DHT dht(DHTPin, DHTType);
 
//// Variables for temperature and humidity
//float temp;
//float humid;

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//// Responder MAC Address (Replace with your responders MAC Address)
//uint8_t broadcastAddress[] = {0x24, 0x0A, 0xC4, 0x04, 0xF4, 0x40};
 
// Define data structure
typedef struct struct_message {
  float a;
  float b;
} struct_message;
 
// Create structured data object
struct_message myData;
 
// Register peer
//esp_now_peer_info_t peerInfo;
 
// Sent data callback function
void OnDataSent(uint8_t *macAddr, uint8_t sendStatus)
{
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
   // Init Serial Monitor
  Serial.begin(115200);
  // Initiate DHT22
  dht.begin();
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
  // Set ESP-NOW role
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Once ESPNow is successfully init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
 
}
 
void loop() {
 
  // Read DHT22 module values
  float temp = dht.readTemperature();
  delay(10);
  float humid = dht.readHumidity();
  delay(10);
  if (isnan(humid) || isnan(temp))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print("Humid: ");
    Serial.print(humid);
 
  // Add to structured data object
    myData.a = temp;
    myData.b = humid;
 
  // Send data
    esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
 
  // Delay for DHT22 sensor
    delay(2000);
  }
}
