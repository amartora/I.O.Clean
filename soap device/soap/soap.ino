#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <HCSR04.h>

//note: D0 is connected to the RST pin in order for the board to be able to wake itself up after sleep. The external wakeup button is a switch connecting D0 to ground.

const char* ssid = "";
const char* password = "";
const char* server = "http://ioclean.xyz/soaptoSQL.php";
String api = "";

#define TRIGPIN 4
#define ECHOPIN 5
#define BUZZER 6 //active buzzer
float maxDist = 5.0; //reading of empty jar in cm

HCSR04 hc(TRIGPIN, ECHOPIN);

float dist1 = 0.0;
float dist2 = 0.0;
float dist3 = 0.0;
float avg = 0.0;
float percentage = 0.0;

void setup() {
  
  Serial.begin(74880);//74880 seems to work best for this board
  pinMode(BUZZER, OUTPUT);
  
  for(int i=0; i<4; i++){
    digitalWrite(BUZZER, HIGH); //beep
    delay(50)
    digitalWrite(BUZZER, LOW)
    delay(4950)
  }

  digitalWrite(BUZZER, HIGH); //beep beep signifying 20 seconds
  delay(50)
  digitalWrite(BUZZER, LOW);
  delay(30);
  digitalWrite(BUZZER, HIGH);
  delay(50)
  digitalWrite(BUZZER, LOW);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  
  if (WiFi.status() == WL_CONNECTED){
    
    Serial.println("Connected to Wi-Fi.");
    
    HTTPClient http;
    http.begin(server);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    delay(1000);

    dist1 = hc.dist(); //returns distance in cm
    Serial.print("Reading 1: ");
    Serial.println(dist1);
    
    delay(1000);

    dist2 = hc.dist();
    Serial.print("Reading 2: ");
    Serial.println(dist2);
    
    delay(1000);

    dist3 = hc.dist();
    Serial.print("Reading 3: ");
    Serial.println(dist3);

    avg = (dist1 + dist2 + dist3) / 3;

    percentage = avg / maxDist;
    
    String httpRequestData = "api_key=" + api + "&percentage=" + percentage;

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
         
  }
  
  else {
    
    delay(1000);
    Serial.println("Not connected to Wi-Fi.");
    
  }
  
  button = 0;
 
  Serial.println("SLEEPING...");
  ESP.deepSleep();

}
void loop() {

}
