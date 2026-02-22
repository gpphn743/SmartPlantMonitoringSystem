#include <SPI.h>
#include <LoRa.h>
#include <TFT_eSPI.h>
#include <DHT.h>

#define LORA_SCK   14   // HSPI_SCLK
#define LORA_MISO  12   // HSPI_MISO
#define LORA_MOSI  13   // HSPI_MOSI
#define LORA_CS     5   // NSS
#define LORA_RST   33   // RST
// #define LORA_DIO0  26   
#define LORA_DIO0  25   // DIO0

#define SOIL_PIN   36     // Soil AOUT
#define LIGHT_PIN  39     // Light AOUT
#define RELAY1_PIN  26     // Relay IN1
#define RELAY2_PIN  27     // Relay IN2
#define DHTPIN  32      
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

const long FREQUENCY    = 923E6;
const unsigned long ACK_TIMEOUT = 500;  

TFT_eSPI tft = TFT_eSPI();

SPIClass LoRaSPI(HSPI);

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Serial2.begin(115200, SERIAL_8N1, 16, 17);

  while (!Serial);

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.println("TFT init successful");

  LoRaSPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

  LoRa.setSPI(LoRaSPI);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(FREQUENCY)) {
    Serial.println("LoRa init failed");
    tft.println("LoRa init failed");
    while (true);
  }
  Serial.println("LoRa Transmitter ready");
  tft.println("LoRa Transmitter ready");

  dht.begin();
  analogSetAttenuation(ADC_11db);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);

  delay(5000);
}

void loop() {
  // LoRa
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float soil = analogRead(SOIL_PIN);
  float light = analogRead(LIGHT_PIN);


  // if (isnan(temperature) || isnan(humidity)) {
  //   Serial.println("Failed to read from DHT sensor!");
  //   tft.fillScreen(TFT_RED);
  //   tft.setTextColor(TFT_WHITE, TFT_RED);
  //   tft.setTextSize(3);
  //   tft.setCursor(10, 50);
  //   tft.println("DHT11 ERROR");
  //   while (true);
  // }
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);                    
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  int y = 0;
  tft.setCursor(0, y);
  tft.printf("Temp : %.1f C\n", temperature);
  y += 20;  // advance down by roughly 20 pixels at size 2
  tft.setCursor(0, y);
  tft.printf("Humid: %.1f %%\n", humidity);
  y += 20;
  tft.setCursor(0, y);
  tft.printf("Moist: %.0f\n", soil);
  y += 20;
  tft.setCursor(0, y);
  tft.printf("Light: %.0f\n", light);
  y += 20;

  String message = String("LoRa :3, Temp :") + String(temperature,1) + 
                   "C, Humid:" + String(humidity,1) + 
                   "%, Moist:" + String((int)soil) + 
                   ", Light:" + String((int)light);

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
  Serial.println(message);
  
  


  // // ACK
  // unsigned long start = millis();
  // bool gotAck = false;
  // while (millis() - start < ACK_TIMEOUT) {
  //   int packetSize = LoRa.parsePacket();
  //   if (packetSize) {
  //     String incoming = LoRa.readString();
  //     if (incoming == "ACK") {
  //       gotAck = true;
  //       break;
  //     }
  //   }
  // }

  // if (gotAck) {
  //   Serial.println("Received ACK");
    
  // } else {
  //   Serial.println("No ACK");
  // }

  if(light < 3700){
    digitalWrite(RELAY1_PIN, HIGH);
    digitalWrite(RELAY2_PIN, HIGH);
    
    }
  else{digitalWrite(RELAY1_PIN, LOW);digitalWrite(RELAY2_PIN, LOW);}
  // digitalWrite(RELAY1_PIN, HIGH);
  // digitalWrite(RELAY2_PIN, HIGH);
  // delay(500);
  // digitalWrite(RELAY1_PIN, LOW);
  // digitalWrite(RELAY2_PIN, LOW);

  // ESP32-CAM
//   String camResult;
//   unsigned long t0 = millis();
//   while (millis() - t0 < 500) {
//   if (Serial2.available()) {
//     String line = Serial2.readStringUntil('\n');
//     if (line.startsWith("RESULT:")) {
//       camResult = line.substring(7);
//       break;
//     }
//   }
// }

//   if (camResult.length()) {
//     Serial.printf("CAM -> %s\n", camResult.c_str());
//     tft.setCursor(0, 40);
//     tft.printf("CAM: %s  ", camResult.c_str());
//   } else {
//     tft.setCursor(0, 40);
//     tft.print("CAM: N/A  ");
//   }

  delay(5000);

  
}
