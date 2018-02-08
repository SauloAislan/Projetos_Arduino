/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>
#include <QList.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid     = "TP-LINK";
const char* password = "senha1234";

float BRed = 0;
float BG = 0;
float BB = 0;

float CR = 0;
float CG = 0;
float CB = 0;

int frequency_red_unmap = 0;
int frequency_green_unmap = 0;
int frequency_blue_unmap = 0;
int frequency_red = 0;
int frequency_green = 0;
int frequency_blue = 0;

char incomingByte = 0;
int32_t  r;
int32_t  g;
int32_t  b;
#define ONE_WIRE_BUS 5
#define S0 4
#define S1 2
#define S2 18
#define S3 19
#define sensorOut 21 
#define Fd 6000L // just typical numbers read by a sensor to ensure ...
#define Fw 55000L  // ... something sensible is returned with no calibration

int seg =0, 
seg2 =0, 
min= 0;
unsigned long  tempo;
unsigned long ult_tempo = 0;
  
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[6];
char temperatureFString[6];

WiFiServer server(80);

QList<float> myCorPuraList;
QList<float> myBrancoList;
QList<float> myUnMapList;
QList<float> myCaliList;
QList<String> myTimeList;


void setup()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  Serial.begin(115200);

  DS18B20.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  
  Serial.println("\nMove the sensor to different color to see the RGB value");
  Serial.println("Note: These values are being read in without sensor calibration");
  Serial.println("and are likely to be far from reality");

  digitalWrite(S0,HIGH);
  digitalWrite(S1,HIGH);
  
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;

void getTemperature() {
  float tempC;
  float tempF;
 // do {
    DS18B20.requestTemperatures(); 
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureCString);
    tempF = DS18B20.getTempFByIndex(0);
    dtostrf(tempF, 3, 2, temperatureFString);
    delay(100);
    Serial.println(tempC);
  //} while (tempC >= 0 || tempC >= (-927.0));
}

void getfrequency() {
  
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency_red
  frequency_red_unmap = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency_red to the RGB Model of 0 to 255
  frequency_red = map(frequency_red_unmap, 25,72,255,0);
  delay(100);
  
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency_green_unmap = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency_green to the RGB Model of 0 to 255
  frequency_green = map(frequency_green_unmap, 30,90,255,0);
  delay(100);
  
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency_blue
  frequency_blue_unmap = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency_blue to the RGB Model of 0 to 255
  frequency_blue = map(frequency_blue_unmap, 25,70,255,0);
  delay(100);
}

void readSensor(){
        
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("<!DOCTYPE HTML>");

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">AQUI</a> PARA CALIBRACAO.<br>");
            client.print("Click <a href=\"/L\">AQUI</a> PARA LEITURA.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
        // your actual web page that displays temperature
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<head></head><body><h1>Espectrometro com ESP8266</h1><h3>Temperatura em Celsius: ");
        //client.println(temperatureCString);
        client.println("*C</h3><h3>Temperatura em Fahrenheit: ");
        //client.println(temperatureFString);

        getfrequency(); 
                
        BRed = frequency_red;
        BG = frequency_green;
        BB = frequency_blue;
        }
        
        if (currentLine.endsWith("GET /L")) {
        
        String stringTwo;
        tempo = millis();
        if((tempo - ult_tempo) >= 1000)
        {
          ult_tempo = tempo;
          seg++;
          if(seg >= 60)
          {
            seg =0;
            min++;
          }}


        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");  // the connection will be closed after completion of the response
        client.println("Refresh: 1");  // refresh the page automatically every 5 sec
        client.println();

        client.print("Tempo: Min ");
        client.print(min);
        client.print(" Seg : ");
        client.print(seg);
//        client.print("  millis: ");      
//        client.println(millis());
        
        stringTwo += min;
        stringTwo += ":";
        stringTwo += seg;
//        stringTwo += ": ";
//        stringTwo.concat(millis());

        myTimeList.push_back(stringTwo);

        
        getfrequency();  
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        client.println("<head></head><body><h1>Espectrometro com ESP8266</h1><h3>Temperatura em Celsius: ");
        //client.println(temperatureCString);
        client.println("*C</h3><h3>Temperatura em Fahrenheit: ");
        //client.println(temperatureFString);
        
        myUnMapList.push_back(frequency_red_unmap);
        myUnMapList.push_back(frequency_green_unmap);
        myUnMapList.push_back(frequency_blue_unmap);
  
        myCorPuraList.push_back(frequency_red);
        myCorPuraList.push_back(frequency_green);
        myCorPuraList.push_back(frequency_blue); 
  
        myBrancoList.push_back(BRed);
        myBrancoList.push_back(BG);
        myBrancoList.push_back(BB);
  
        CR =  frequency_red / BRed * 255;
        CG = frequency_green / BG * 255;
        CB = frequency_blue / BB * 255;
        
        myCaliList.push_back(CR);
        myCaliList.push_back(CG);
        myCaliList.push_back(CB);

        client.println("</br>"); 
        client.println("Lista de Intervalos"); 
        for(int t=0;t<myTimeList.size();t++)
        {
        client.println("[");
        client.println(myTimeList.at(t));
        client.println("] ");
        }
        
        client.print("<table><tr><td>--------CorPura--------</td></tr>");
        client.print("<tr><td width=200px height=200px style='background-color:rgb(");//printing name
        client.print((int) frequency_red);
        client.print(",");
        client.print((int) frequency_green);
        client.print(",");
        client.print((int) frequency_blue);
        client.print(")'></td><td>");
        
        for(int j=0;j<myCorPuraList.size();j++)
        {
        client.println(myCorPuraList.at(j));
        }
  
        client.println("</td></tr>");
        client.println("");
  
        client.print("<tr><td>--------Calibrado--------</td></tr>");
        client.print("<tr><td width=200px height=200px style='background-color:rgb(");//printing name
        client.print((int) CR);
        client.print(",");
        client.print((int) CG);
        client.print(",");
        client.print((int) CB);
        client.print(")'></td><td>");
        
        for(int m=0;m<myCaliList.size();m++)
        {
        client.println(myCaliList.at(m));
        }
  
        client.println("</td></tr></table>");
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void loop() 
{
  readSensor();
}
