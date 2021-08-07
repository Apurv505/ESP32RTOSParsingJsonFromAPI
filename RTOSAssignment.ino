/*       Written by- Apurv Kumar     
This contains Esp32 RTOS code to get JSON data from the API, de-serialize it, store i and search any key present in the data.*/


#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include <SD.h>
#include <SPI.h>

#define WIFI_NETWORK "stationName"                           ///////Enter WiFi username and password/////////////
#define WIFI_PASSWORD "password"
#define WIFI_TIMEOUT_MS 20000

const int capacity = 40901;
const int chipSelect = 4;
const char *filename = "/test.jso";
File myFileSDCard;

const static String serverUrl= "**********Enter server url here**************";

StaticJsonDocument<capacity> doc;

TaskHandle_t downloadDataHndl;

HTTPClient http;

void keepWiFiAlive(void * parameter) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      //      Serial.println("Wifi still connected");
      vTaskDelay(10000 / portTICK_PERIOD_MS);
      continue;
    }

    Serial.println("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {}

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WIFI] FAILED");
      vTaskDelay(20000 / portTICK_PERIOD_MS);
    }

    Serial.println("[WIFI] CONNECTED: " + WiFi.localIP());
    HTTPClient http;
    Serial.println("Downloading and processing data");




  }

}

void readSerial(void * parameter) {
  for (;;) {
    if (Serial.available() ) {
      unsigned int value = 0;
      long unsigned int startTime = millis();
      String key = Serial.readString();

      long unsigned int endTime = millis();

      Serial.println("");
      Serial.print("Key is: ");
      Serial.println(key);

      value = doc[key].as<int>();

      if (value == 0) {
        Serial.println("Key not found. Enter the correct key.");
      }
      else {

        Serial.print("Value is: ");
        Serial.println(value);

        Serial.print("Query time: ");
        Serial.print(endTime - startTime);
        Serial.println(" ms");

        Serial.println("\nWaiting for the key to be entered...\n");

      }
    }
  }
}


void downloadData(void * parameter) {
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {

     
      http.useHTTP10(true);
      http.begin(serverUrl);
      http.GET();
      Serial.println("\nParsing start\n");
      DeserializationError err = deserializeJson(doc, http.getStream());

      if (err) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(err.f_str());
      }
      else {

        JsonObject root = doc.as<JsonObject>();
        int i = 1;
        for (JsonPair p : root) {
          if (i > 10) {
            break;
          }
          else {
            const char* key = p.key().c_str();
            int value = p.value();
            Serial.print(key); Serial.print(" : "); Serial.println(value);
            i++;
          }
        }
        http.end();
        Serial.println("");
        Serial.println("\nDeleting dataDownload task\n");
        Serial.println("\nWaiting for the key to be entered...\n");
        vTaskDelete(downloadDataHndl);

      }
    }

  }

}


void writeToSdCard(void * parameter) {
  
//  Serial.println(F("Opened file in write mode"));
//  myFileSDCard = SD.open(filename, FILE_WRITE);
//  
//  if (myFileSDCard) {
//    Serial.print(F("Filename: "));
//    Serial.println(filename);
//
//    Serial.print(F("Starting writing..."));
//
//    serializeJson(doc, myFileSDCard);
//
//    Serial.print(F("..."));
//   
//    myFileSDCard.close();
//    Serial.println(F("done."));
//
//    
//  } else {
//   
//    Serial.print(F("Error opening "));
//    Serial.println(filename);
//  }
}




void readFromSdCard(void * parameter) {

//  myFileSDCard = SD.open(filename);
//  if (myFileSDCard) {
//    
//    //          if (myFileSDCard.available()) {
//    //              firstWrite = false;
//    //          }
//
//    DeserializationError error = deserializeJson(doc, myFileSDCard);
//    
//    if (error) { 
//      Serial.print(F("Error parsing JSON "));
//      Serial.println(error.c_str());
//    }
//
//    myFileSDCard.close();
//
//    
//  } else {
//    
//    Serial.print(F("Error opening (or file does not exists) "));
//    Serial.println(filename);
//
//    Serial.println(F("Empty json created"));
//    
//  }


}

void setup() {
  Serial.begin(115200);

  if (!SD.begin(chipSelect)) {
    Serial.println(F("Failed to initialize SD library"));
 
  }



  xTaskCreatePinnedToCore(
    downloadData,
    "Download Data",
    capacity,
    NULL,
    1,
    &downloadDataHndl,
    CONFIG_ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    keepWiFiAlive,
    "Keep WiFi Alive",
    5000,
    NULL,
    1,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    readSerial,
    "Read Serial Data",
    1024,
    NULL,
    1,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE);

//    xTaskCreatePinnedToCore(
//    readFromSdCard,
//    "Read file from SD card",
//    capacity,
//    NULL,
//    1,
//    NULL,
//    CONFIG_ARDUINO_RUNNING_CORE);
//
//    xTaskCreatePinnedToCore(
//    writeToSdCard,
//    "Write to SD card",
//    capacity,
//    NULL,
//    1,
//    NULL,
//    CONFIG_ARDUINO_RUNNING_CORE);


}

void loop() {
}
