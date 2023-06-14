#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <BlynkSimpleEsp8266.h>
#include <NTPClientWrapper.h>
// #include <timer.h>
//  #include <NTPClient.h>
//  #include <WiFiUdp.h>

// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "ph.pool.ntp.org");
NTPClientWrapper timeClient(28800);

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#define DATABASE_URL ""
#define API_KEY ""
#define FIREBASE_PROJECT_ID ""

#define CS_PIN D10    // LoRa radio chip select
#define RESET_PIN D14 // LoRa radio reset
#define IRQ_PIN D2    // change for your board; must be a hardware interrupt pin

#define WATER_PUMP_PIN 0
#define FAN_PIN 2

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
BlynkTimer timer;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

unsigned long dataMillis = 0;

float humidity = 0.0;
float temperature = 0.0;
float heatIndex = 0.0;
float lux = 0.0;
float phVoltage = 0.0;
float ph = 0.0;
int soilMoistureAnalog = 0;
int soilMoisturePercentage = 0;
int soilMoistureThreshold = 0;
int nitrogen = 0;
int phosphorous = 0;
int potassium = 0;

const unsigned long CHECK_TIME = 30000;
unsigned long lastMsgTime = 0;

void blynkCallback()
{
  Blynk.virtualWrite(V4, ph);
  Blynk.virtualWrite(V11, temperature);
  Blynk.virtualWrite(V12, humidity);
  Blynk.virtualWrite(V13, lux);
  Blynk.virtualWrite(V14, soilMoisturePercentage);
  Blynk.virtualWrite(V15, heatIndex);
  Blynk.virtualWrite(V16, nitrogen);
  Blynk.virtualWrite(V17, phosphorous);
  Blynk.virtualWrite(V18, potassium);
}

// The Firestore payload upload callback function
void fcsUploadCallback(CFS_UploadStatusInfo info)
{
  if (info.status == fb_esp_cfs_upload_status_init)
  {
    Serial.printf("\nUploading data (%d)...\n", info.size);
  }
  else if (info.status == fb_esp_cfs_upload_status_upload)
  {
    Serial.printf("Uploaded %d%s\n", (int)info.progress, "%");
  }
  else if (info.status == fb_esp_cfs_upload_status_complete)
  {
    Serial.println("Upload completed ");
  }
  else if (info.status == fb_esp_cfs_upload_status_process_response)
  {
    Serial.print("Processing the response... ");
  }
  else if (info.status == fb_esp_cfs_upload_status_error)
  {
    Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
  }
}

// SPI Reset
void checkTimeout()
{
  unsigned long currentTime = millis();

  if (currentTime - lastMsgTime >= CHECK_TIME)
  {
    lastMsgTime = currentTime;
    Serial.println("No data received...");

    LoRa.end();
    // LoRa.setPins(ss, rst, dio0);
    LoRa.setPins(CS_PIN, RESET_PIN, IRQ_PIN); // set CS, reset, IRQ pin

    if (!LoRa.begin(433E6))
    {
      Serial.println("Resting LoRa failed!");
    }
    else
    {
      Serial.println("Resting LoRa ok");
    }
  }
}

void setup()
{

  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("LoRa Receiver");

  LoRa.setPins(CS_PIN, RESET_PIN, IRQ_PIN); // set CS, reset, IRQ pin

  if (!LoRa.begin(433E6))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  LoRa.setSyncWord(0xF3); // ranges from 0-0xFF, default 0x34, see API docs
  Serial.println("LoRa init succeeded.");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi ");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Connected with Firebase...");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  // timeClient.begin();
  // timeClient.setTimeOffset(28800);
  timeClient.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  if (Blynk.connected())
  {
    Serial.println("Connected to Blynk...");
  }

  timer.setInterval(1000L, blynkCallback);
  config.cfs.upload_callback = fcsUploadCallback;
}

void loop()
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    lastMsgTime = millis();
    // received a packet
    Serial.printf("Received packet with size %d\n", packetSize);

    // Allocate a buffer for the incoming packet
    StaticJsonDocument<256> jsonBuffer;
    DeserializationError error = deserializeJson(jsonBuffer, LoRa);
    if (error)
    {
      Serial.println("Error: Failed to parse JSON message");
      return;
    }
    // Pretty-print the JSON message to the serial monitor
    serializeJsonPretty(jsonBuffer, Serial);
    Serial.println();
    Serial.println(timeClient.getCurrentDate());

    humidity = jsonBuffer["Humidity"];
    temperature = jsonBuffer["Temperature"];
    heatIndex = jsonBuffer["Heat_Index"];
    lux = jsonBuffer["Lux"];
    ph = jsonBuffer["pH"];
    soilMoisturePercentage = jsonBuffer["Soil_Moisture_Percentage"];
    nitrogen = jsonBuffer["Nitrogen"];
    phosphorous = jsonBuffer["Phosphorous"];
    potassium = jsonBuffer["Potassium"];

    String formatted_Time = timeClient.getFormattedTime();
    String current_Date = timeClient.getCurrentDate();
    String time = current_Date + "T" + formatted_Time + "+08:00";
    String EpochTime = String(timeClient._getEpochTime());
    int id = int(timeClient._getEpochTime());

    if (Firebase.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
      dataMillis = millis();

      // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create.ino
      FirebaseJson content;

      String documentPath = "sensors_version2/data" + EpochTime;
      content.set("fields/id/integerValue", id);

      content.set("fields/humidity/doubleValue", humidity);

      content.set("fields/temperature/doubleValue", temperature);
      content.set("fields/heatIndex/doubleValue", heatIndex);
      content.set("fields/lux/doubleValue", lux);
      content.set("fields/pH/doubleValue", ph);
      content.set("fields/soilMoisturePercentage/integerValue", soilMoisturePercentage);
      content.set("fields/Nitrogen/integerValue", nitrogen);
      content.set("fields/Phosphorous/integerValue", phosphorous);
      content.set("fields/Potassium/integerValue", potassium);
      content.set("fields/Timestamp/timestampValue", time); // RFC3339 UTC "Zulu" format
      Serial.print("Creating a document... ");
      Serial.println(time + " " + Firebase.getCurrentTime());

      if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw()))
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      else
        Serial.println(fbdo.errorReason());
    }

    Serial.print("with RSSI ");
    Serial.println(LoRa.packetRssi());
  }

  checkTimeout();
  timeClient.update();
  Blynk.run();
  timer.run();
}