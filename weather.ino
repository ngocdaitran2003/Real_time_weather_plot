#include "DHT.h"
#include <HardwareSerial.h>

#define DHTPIN 14         // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#define BUFFER_SIZE (20u)
char buffer[BUFFER_SIZE];

#define RX2_PIN 16
#define TX2_PIN 17

HardwareSerial SerialPort(2); // use UART2

void Print_Values(){
  uint8_t temperature = (uint8_t)dht.readTemperature();
  uint8_t humidity = (uint8_t)dht.readHumidity();
  int n = snprintf(buffer, BUFFER_SIZE, "%.2d,%.2d\r\n", temperature, humidity);
  SerialPort.print(buffer);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();
  SerialPort.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
}

void loop() {
  // put your main code here, to run repeatedly:
  Print_Values();
  delay(1000);
}
