#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "DHT.h"
#include "DS3231.h"
#include <EEPROM.h>

#define DHTPIN 2//this is pin D4 on the NodeMCU
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

RTClib RTC;

//note: D0 is connected to the RST pin in order for the baord to be able to wake itself up after sleep. The external wakeup button is a push button connecting D0 to ground.

const char* ssid = " ";
const char* password = " ";
const char* server = "http://ioclean.xyz/fridgetoSQL.php"; // change to whatever it will be called
String api = " ";

int button = 0;
int DHTpower = 13;//D7
uint32_t pressedTime;
uint32_t lastAwake;
int sleepTime = 30;//in seconds
uint32_t counter;


void writeLongIntoEEPROM(int address, long number)//since EEPROm can only have 1 byte (8 bits) stored at each address, custom functions like these are needed
{ 
  EEPROM.write(address, (number >> 24) & 0xFF);
  EEPROM.write(address + 1, (number >> 16) & 0xFF);
  EEPROM.write(address + 2, (number >> 8) & 0xFF);
  EEPROM.write(address + 3, number & 0xFF);
}
long readLongFromEEPROM(int address)
{
  return ((long)EEPROM.read(address) << 24) +
         ((long)EEPROM.read(address + 1) << 16) +
         ((long)EEPROM.read(address + 2) << 8) +
         (long)EEPROM.read(address + 3);
}

void setup() {
  
  DateTime now = RTC.now();
  
  Wire.begin();
  Serial.begin(74880);//74880 seems to work best for this board
  dht.begin();
  EEPROM.begin(512);
  
  pinMode(DHTpower, OUTPUT);
 
  button = 0;
  
  pressedTime = readLongFromEEPROM(0);
  lastAwake = readLongFromEEPROM(10);
  
  now = RTC.now();
  Serial.print("Asleep for: ");
  Serial.println(now.unixtime()-lastAwake);
  if ((now.unixtime()-lastAwake) < (sleepTime-2)){
   button = 1;
  }

  if (button == 1){
    pressedTime = now.unixtime();
    writeLongIntoEEPROM(0, pressedTime);//writing pressedTime to flash memory so it doesn't get wiped after a reset
  }
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }

  digitalWrite (DHTpower, LOW);
  now = RTC.now();
  
  if (WiFi.status() == WL_CONNECTED){
    
    Serial.println("Connected to Wi-Fi.");
    
    HTTPClient http;
    http.begin(server);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    digitalWrite (DHTpower, HIGH);//powering on DHT22, for some reason DHT22 wouldn't read if it is powered from boot-up, so here it gets powered after a bit
    
    delay(1000);
    
    float temperature = dht.readTemperature(true);
    float humidity = dht.readHumidity();

    counter = now.unixtime() - pressedTime;
        
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°F");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.print("Button: ");
    Serial.println(button);
    Serial.print("Pressed: ");
    Serial.println(pressedTime);
    Serial.print("Now: ");
    Serial.println(now.unixtime());
    Serial.print("Time since: ");
    Serial.println(counter);
    Serial.print("Last awake: ");
    Serial.println(lastAwake);
    

    String httpRequestData = "api_key=" + api + "&temp=" + temperature + "&humid=" + humidity + "&counter=" + counter;

    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode>0) {
        Serial.print("HTTP Response: ");
        Serial.println(httpResponseCode);   
    }
      
    else {
        Serial.print("Error: ");
        Serial.println(httpResponseCode);
    }
      
    http.end();
     
    //delay(10000); I don't believe this is needed, but we will see...
    
  }
  
  else {
    
    delay(1000);
    Serial.println("Not connected to Wi-Fi.");
    
  }
  
  button = 0;
  
  digitalWrite (DHTpower, LOW);
  
  now = RTC.now();
  lastAwake = now.unixtime();//recording the time the ESP was last awake so on wake-up, can know if it was woken up early (by an outside source)
  writeLongIntoEEPROM(10, lastAwake);
  
  EEPROM.commit(); 
  Serial.println("SLEEPING...");
  ESP.deepSleep(sleepTime * 1e6); // 1 minute in microseconds, ake sure D0 is connected to RST

}
void loop() {

}
