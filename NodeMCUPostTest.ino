/*
Rui Santos
Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

*/

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#endif

#include <Wire.h>


// Replace with your network credentials
const char* ssid     = " ";
const char* password = " ";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = " ";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = " ";

int butToggle;
int lastButtonState;    
int currentButtonState; 
int counter; // counter is implemented so that button and potentiometer wil be read often, without sending too many HTTP posts
float potValue;

  void setup() {
  Serial.begin(9600); // set 11250 for Heltec WiFi Kit 8;
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  currentButtonState = digitalRead(0);
  
  butToggle = currentButtonState;
  
  counter = 0;
}

  void loop() {
  //Check WiFi connection status
  
  if(WiFi.status()== WL_CONNECTED){
  HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    float rawPot = analogRead(0);
    potValue = (rawPot/1024)*100;
    
    //button toggle code
    lastButtonState    = currentButtonState;     
    currentButtonState = digitalRead(0);
    
    if(lastButtonState == HIGH && currentButtonState == LOW) {
    
    Serial.println("The button is pressed");
    
    butToggle = !butToggle;
    }
    
   
    //button toggle code
    
    
    if (counter >= 500){ //change the number here to change how often data will be posted thru HTTP
    
      // Prepare your HTTP POST request data
      String httpRequestData = "api_key=" + apiKeyValue + "&potValue=" + potValue + "&butToggle=" + butToggle;
      
      Serial.print("httpRequestData: ");
      Serial.println(httpRequestData);
      
      // You can comment the httpRequestData variable above
      // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
      //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";
      
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
      
      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      }
      
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      
      // Free resources
      http.end();
      
      counter = 0;
    }
  
  }
  
  else {
    Serial.println("WiFi Disconnected");
  }
  
  Serial.print ("COUNTER: ");
  Serial.println(counter);
  Serial.print ("POTENTIOMETER: ");
  Serial.println(potValue);
  Serial.print ("BUTTON: ");
  Serial.println(butToggle);
  
  counter++;
  
  delay(10);
}
