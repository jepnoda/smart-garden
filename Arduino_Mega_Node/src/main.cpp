#include <Arduino.h>
#include <SPI.h> // include libraries
#include <LoRa.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Light.h>
#include <Ph.h>
#include <Soil.h>

#include <SoftwareSerial.h>
#include <Wire.h>

#define DHTPIN 6
#define DHTTYPE DHT11
#define LIGHT_PIN A0
#define PH_PIN A7
#define PH_OFFSET 0.0
#define SOIL_PIN A2

const int csPin = 53;   // LoRa radio chip select
const int resetPin = 4; // LoRa radio reset
const int irqPin = 2;   // change for your board; must be a hardware interrupt pin

DHT dht(DHTPIN, DHTTYPE);
Light light(LIGHT_PIN);
Ph ph(PH_PIN, PH_OFFSET);
Soil soil(581, 324, SOIL_PIN);

// RE and DE Pins set the RS485 module
// to Receiver or Transmitter mode
#define RE 8
#define DE 7

// Modbus RTU requests for reading NPK values
const byte nitro[] = {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

// A variable used to store NPK values
byte values[11];

// Sets up a new SoftwareSerial object
// Digital pins 10 and 11 should be used with a Mega or Mega 2560
// SoftwareSerial mod(2, 3);
SoftwareSerial mod(10, 11);

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

byte nitrogen()
{
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(nitro, sizeof(nitro)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++)
    {
      values[i] = mod.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}

byte phosphorous()
{
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(phos, sizeof(phos)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++)
    {
      values[i] = mod.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}

byte potassium()
{
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if (mod.write(pota, sizeof(pota)) == 8)
  {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++)
    {
      values[i] = mod.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}

void setup()
{

  Serial.begin(9600); // initialize serial
  while (!Serial)
    ;
  dht.begin();
  light.begin();
  ph.begin();
  soil.begin();
  // Set the baud rate for the SerialSoftware object
  mod.begin(9600);

  // Define pin modes for RE and DE
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  delay(500);

  Serial.println("LoRa Duplex - Set sync word");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin

  if (!LoRa.begin(433E6))
  { // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true)
      ; // if failed, do nothing
  }

  LoRa.setSyncWord(0xF3); // ranges from 0-0xFF, default 0x34, see API docs
  Serial.println("LoRa init succeeded.");
}

void loop()
{

  if (runEvery(5000))
  {
    byte val1, val2, val3;
    val1 = nitrogen();
    delay(250);
    val2 = phosphorous();
    delay(250);
    val3 = potassium();
    delay(250);

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    float heatIndex = dht.computeHeatIndex();
    float lux = light.getLux();
    float phVoltage = ph.readVoltage();
    float phValue = ph.readpH();
    int soilMoistureAnalog = soil.readSensorValue();
    int soilMoisturePercentage = soil.calculateSoilMoisturePercent();

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["Humidity"] = humidity;
    jsonDoc["Temperature"] = temperature;
    jsonDoc["Heat_Index"] = heatIndex;
    jsonDoc["Lux"] = lux;
    jsonDoc["pH"] = phValue;
    jsonDoc["Soil_Moisture_Percentage"] = soilMoisturePercentage;
    jsonDoc["Nitrogen"] = val1;
    jsonDoc["Phosphorous"] = val2;
    jsonDoc["Potassium"] = val3;

    LoRa.beginPacket();
    serializeJson(jsonDoc, LoRa);
    LoRa.endPacket();

    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Heat Index: ");

    Serial.println(heatIndex);
    Serial.print("Lux: ");

    Serial.println(lux);
    Serial.print("pH Voltage: ");

    Serial.println(phVoltage);
    Serial.print("pH Value: ");

    Serial.println(phValue);
    Serial.print("Soil Moisture Analog Value: ");

    Serial.println(soilMoistureAnalog);
    Serial.print("Soil Moisture Percentage: ");

    Serial.println(soilMoisturePercentage);
    Serial.print("Nitrogen: ");
    Serial.print(val1);
    Serial.println(" mg/kg");
    Serial.print("Phosphorous: ");
    Serial.print(val2);
    Serial.println(" mg/kg");
    Serial.print("Potassium: ");
    Serial.print(val3);
    Serial.println(" mg/kg");
  }
}