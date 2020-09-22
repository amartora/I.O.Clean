#include <ESP8266WiFi.h> //for WiFi functions
#include <SPI.h> // for OLED
#include <Wire.h> // for OLED
#include <Adafruit_SSD1306.h> // for OLED

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET 16 // RST pin for OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup(){
  
  
  Serial.begin(115200);
  Serial.println();




   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    
  display.clearDisplay();
  display.display(); // the display() command basically pushes everything you drew to the display... that way you can draw many shapes
  // over the course of many lines of code, and the use the command to push them all at once to show up at the same time on teh display

  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  



  WiFi.begin("     ", "     "); // network name, password

  Serial.print("Connecting");
  
  display.println(F("Connecting"));
  display.display();
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    display.print(F("."));
    display.display();
  }
  Serial.println();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.display();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  display.println(F("Connected"));
  display.println(F("IP address:"));
  display.println((WiFi.localIP()));
  display.display();
}


void loop() {
}
