/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-post-ifttt-thingspeak-arduino/ 

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

 I used this guide to figure out how to make https requests with ThingSpeak for my project 
          - Jacob Noble
*/

#include <WiFi.h>
#include <HTTPClient.h>

#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT11

//wifi validation
// Replace These values with your own
char ssid[] = "Replace these with your own values";
char pass[] = "Replace these with your own values";
int status = WL_IDLE_STATUS;

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
// Service API Key
// Replace These values with your own
String apiKeyDHT = "Replace these with your own values";
String myWriteAPIKEYMotion = "Replace these with your own values";

//Timer Settings for Device
unsigned long lastTime = 0;
// Set timer to 10 minutes
unsigned long timerDelay = 600000;
// Timer set to 10 seconds
//unsigned long timerDelay = 10000;

// Define the DHT
DHT dht(DHTPIN, DHTTYPE, 20);

//Define the PIR
int t = 20;
int pir;
int pinPIR = 19; 
int pirMax;

void setup() {
  Serial.begin(115200); //Set the baud rate to 115200
  delay(10);
  
  WiFi.begin(ssid, pass);
  Serial.println("connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi Network with address: ");
  Serial.println(WiFi.localIP());

  // change maintainence message based on what's being tested
  Serial.println("Timer set to 10 minutes (timerDelay variable), it will take 10 minutes before publishing the first reading.");

  dht.begin();
  pinMode(pinPIR, INPUT);
}

void loop() {
  // Read and convert temp/humidity
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  float tempConvert = (temp * (9/5)) + 32;
  
  //this is the second part of the post string for Thingspeak
  String postStrDHT = "&field1=" + String(tempConvert) + "&field2=" + String(humidity);

  //A loop that continually reads for a change in movement for 20 seconds. If any movement is found in those 20 seconds, the reading will be set to 1.
  pirMax=0;
  for (int i=0; i < t; i++){
        
        pir= digitalRead(pinPIR);
       
        if(pir>pirMax)pirMax=pir;
        delay(1000);
     } 
     
  pir=pirMax;

  //post string for the PIR
  String postStrPIR = "&field1=" + String(pir);
  
  //Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestDataDHT = "api_key=" + apiKeyDHT + postStrDHT;           
      // Send HTTP POST request
      int httpResponseCodeDHT = http.POST(httpRequestDataDHT);
     
      Serial.print("HTTP Response code for DHT: ");
      Serial.println(httpResponseCodeDHT);

      String httpRequestDataPIR = "api_key=" + myWriteAPIKEYMotion + postStrPIR; 
      int httpResponseCodePIR = http.POST(httpRequestDataPIR);

      Serial.print("HTTP Response code for PIR: ");
      Serial.println(httpResponseCodePIR);
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  delay(8000); // thingspeak needs a minimum 15 second delay between updates
}
