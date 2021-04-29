#include <SoftwareSerial.h>

#include "Picaso_Serial_4DLib.h"
#include "Picaso_Const4D.h"
#define DisplaySerial Serial1

#define RESETLINE     30
#define DEBUG true
Picaso_Serial_4DLib Display(&DisplaySerial);

#define SSID        "WhywasRouterreleasedfromprison"          // add you SSID here
#define PASSWORD    "He had high speed connections"          // add your router password here



// ****************************************************
// Motor Controller Ports
// ****************************************************
#define pwmA 10
#define dirA1 12 // 8
#define dirA2 11 // 9

#define pwmB 7
#define dirB1 5
#define dirB2 6

#define pwmC 4
#define dirC1 2
#define dirC2 3

String data;

String server = "ioclean.xyz"; // www.example.com

String uri = "/robottoSQL.php";// our example is /ESPserialpost.php

// ESP Setup
#define ESPRESET 17
#include <SoftwareSerial.h>
#define ESPserial SerialS
SoftwareSerial SerialS(8, 9) ;
// Uncomment next 2 lines to use ESP8266 with ESP8266 library from https://github.com/itead/ITEADLIB_Arduino_WeeESP8266
#include "ESP8266.h"
ESP8266 wifi(SerialS, 19200);


// ****************************************************
// Motor Controllers
// ****************************************************



// Globals
double sideStep = 0.49; // Limiting factor to ensure direct side to side movement
double side = 0.50;
int totMove = 0;
unsigned long totDuration;


void mycallback(int ErrCode, unsigned char Errorbyte)
{
#ifdef LOG_MESSAGES
  const char *Error4DText[] = {"OK\0", "Timeout\0", "NAK\0", "Length\0", "Invalid\0"} ;
  Serial.print(F("Serial 4D Library reports error ")) ;
  Serial.print(Error4DText[ErrCode]) ;
  if (ErrCode == Err4D_NAK)
  {
    Serial.print(F(" returned data= ")) ;
    Serial.println(Errorbyte) ;
  }
  else
    Serial.println(F("")) ;
  while (1) ; // you can return here, or you can loop
#else
  // Pin 13 has an LED connected on most Arduino boards. Just give it a name
#define led 13
  while (1)
  {
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);                // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(200);                // wait for a second
  }
#endif
}
// end of routine to handle ESP Serial errors


// ****************************************************
// Stops the motors
// RETURNS: none
// ****************************************************



void setup()
{

  // Ucomment to use the Serial link to the PC for debugging
  // Serial.begin(115200) ;        // serial to USB port
  // Note! The next statement will stop the sketch from running until the serial monitor is started
  //       If it is not present the monitor will be missing the initial writes
  //    while (!Serial) ;             // wait for serial to be established

  pinMode(RESETLINE, OUTPUT);       // Display reset pin
  digitalWrite(RESETLINE, 1);       // Reset Display, using shield
  delay(100);                       // wait for it to be recognised
  digitalWrite(RESETLINE, 0);       // Release Display Reset, using shield
  // Uncomment when using ESP8266
  pinMode(ESPRESET, OUTPUT);        // ESP reset pin
  digitalWrite(ESPRESET, 1);        // Reset ESP
  delay(100);                       // wait for it t
  digitalWrite(ESPRESET, 0);        // Release ESP reset
  delay(3000) ;                     // give display time to startup

  // now start display as Serial lines should have 'stabilised'
  DisplaySerial.begin(200000) ;     // Hardware serial to Display, same as SPE on display is set to
  Display.TimeLimit4D = 5000 ;      // 5 second timeout on all commands
  Display.Callback4D = mycallback ;



  ESPserial.begin(115200) ;         // assume ESPserial set to 115200 baud, it's default setting
  // what we need to do is attempt to flip it to 19200
  // the maximum baud rate at which software serial actually works
  // if we run a program without resetting the ESP it will already be 19200
  // and hence the next command will not be understood or executed
  ESPserial.println("AT+UART_CUR=19200,8,1,0,0\r\n") ;
  ESPserial.end() ;
  delay(10) ;                         // Necessary to allow for baud rate changes
  ESPserial.begin(19200) ;            // start again at a resonable baud rate

  Display.gfx_ScreenMode(LANDSCAPE) ; // Put in Landscape (as AP list is too wide for Portrait)
  Display.gfx_Cls() ;                 // clear the screen
  Display.txt_FGcolour(LIME) ;
  Display.txt_FontID(FONT3) ;         // largest internal font
  Display.putstr("Welcome!\n") ;

  if (wifi.setOprToStationSoftAP()) {
    Display.print("to station Soft AP ok\r\n");
  } else {
    Display.print("to station Soft AP err\r\n");
  }

  // ESPserial.println("AT+CIPSTA=\"10.0.0.82\"\r\n") ;

  if (wifi.joinAP(SSID, PASSWORD)) {
    Display.print("Join AP success\r\n");
    Display.print("IP: ");
    Display.println(wifi.getLocalIP().c_str());
  } else {
    Display.print("Join AP failure\r\n");
  }

  if (wifi.enableMUX()) {
    Display.print("multiple ok\r\n");
  } else {
    Display.print("multiple err\r\n");
  }

  if (wifi.startServer(45777)) {   // configured the module to answer web requests on port 80
    Display.print("start tcp server ok\r\n");
  } else {
    Display.print("start tcp server err\r\n");
  }

  Display.println("Server Ready\n");
  Display.println("Now enter the STAIP/pinx in url field of the intenet browser to toggle LED");
  Display.println("");
  Display.println("e.g : 10.0.0.82/pin=13 ----- Toggles LED connected to D13");






}


void loop()
{
  String api_key = "1234567890987";
  Display.gfx_Cls() ;
  data = "api_key=" + api_key + "&energy=" + totMove + "&duration=" + totDuration;
  Display.println(data);


  String httpHeader;
  httpHeader = "POST ";
  httpHeader += "/robottoSQL.php HTTP/1.1\r\n";
  httpHeader += "Host: ioclean.xyz\r\n";
  httpHeader += "Content-Type: application/x-www-form-urlencoded\r\n";
  httpHeader += "Content-Length: ";
  httpHeader += data.length();
  httpHeader += "\r\n\r\n";

  String final = httpHeader + data;

  
  SendToMod(final);

  

  delay(3000);
  // httppost();

  delay(1000);
}

/*
  Name: sendData
  Description: Function used to send data to ESP8266.
  Params: command - the data/command to send; timeout - the time to wait for a rESPserialonse; debug - print to Serial window?(true = yes, false = no)
  Returns: The rESPserialonse from the ESPserial8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
  String rESPserialonse = "";

  int dataSize = command.length();
  char data[dataSize];
  command.toCharArray(data, dataSize);

  ESPserial.write(data, dataSize); // send the read character to the ESPserial8266
  if (debug)
  {
    Serial.println("\r\n====== HTTP RESPserialonse From Arduino ======");
    Serial.write(data, dataSize);
    Serial.println("\r\n========================================");
  }

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (ESPserial.available())
    {

      // The ESPserial has data so display its output to the serial window
      char c = ESPserial.read(); // read the next character.
      rESPserialonse += c;
    }
  }

  if (debug)
  {
    Display.print(rESPserialonse);
  }

  return rESPserialonse;
}

/*
  Name: sendHTTPRESPserialonse
  Description: Function that sends HTTP 200, HTML UTF-8 rESPserialonse
*/
void sendHTTPRESPserialonse(int connectionId, String content)
{

  // build HTTP rESPserialonse
  String httpRESPserialonse;
  String httpHeader;
  // HTTP Header
  httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n";
  httpHeader += "Content-Length: ";
  httpHeader += content.length();
  httpHeader += "\r\n";
  httpHeader += "Connection: close\r\n\r\n";
  httpRESPserialonse = httpHeader + content + " "; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
  sendCIPData(connectionId, httpRESPserialonse);

}

/*
  Name: sendCIPDATA
  Description: sends a CIPSEND=<connectionId>,<data> command

*/
void sendCIPData(int connectionId, String data)
{
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += data.length();
  cipSend += "\r\n";
  ESPserial.println(cipSend);
  delay(300);
  sendData(data, 1000, DEBUG);
}

void SendToMod(String content)
{
  String cipSend = "AT+CIPSEND=";
  cipSend += content.length()+1;
  cipSend += "\r\n";
  content += " "; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
  String response = "";

  ESPserial.print(cipSend); // send the read character to the esp8266
  delay(20);
  sendData(content, 1000, 3000);
  /*
  String res = sendCommand(cipSend,1000);
  if(res.indexOf('>')!=-1)
    sendData(content, 1000);
  else
   Display.print("\r\nERROR");
   */
}


//void httppost () {
//
//  ESPserial.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");//start a TCP connection.
//
//  // if ( ESPserial.find("OK")) {
//
//   // Display.println("TCP connection ready");
//
// //  }
// 
//  delay(1000);
//
//  String postRequest =
//
//    "GET " + uri + " HTTP/1.1\r\n" +
//
//    "Host: " + server + "\r\n" +
//
//    "Accept: *" + "/" + "*\r\n" +
//
//    "Content-Length: " + data.length() + "\r\n" +
//
//    "Content-Type: application/x-www-form-urlencoded\r\n" +
//
//    "\r\n" + data;
//
//  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
//
//  ESPserial.print(sendCmd);
//
//  ESPserial.println(postRequest.length() );
//
//  delay(500);
//
//  if (ESPserial.find(">")) {
//    Display.println("Sending.."); ESPserial.print(postRequest);
//
//    if ( ESPserial.find("SEND OK")) {
//      Display.println("Packet sent");
//
//      while (ESPserial.available()) {
//
//        String tmpRESPserial = ESPserial.readString();
//
//        Display.println(tmpRESPserial);
//
//      }
//
//      // close the connection
//
//      ESPserial.println("AT+CIPCLOSE");
//
//    }
//
//  }
//  delay(2000);
//}
