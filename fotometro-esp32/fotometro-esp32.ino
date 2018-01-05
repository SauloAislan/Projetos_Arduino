#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
  Para funcionar tem q usar a Placa ESP32 DEV module
*********/

// Including the ESP8266 WiFi library

#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network details

char* ssid     = "xxxxxxxxxx";
char* password = "xxxxxxxxxx";

// Data wire is plugged into pin D1 on the ESP8266 12-E - GPIO 5
#define ONE_WIRE_BUS 5
//#define LED 3
#define S0 4
#define S1 2
#define S2 18
#define S3 19
#define sensorOut 21
int frequency_red = 0;
int frequency_green = 0;
int frequency_blue = 0;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[6];
char temperatureFString[6];

// Web Server on port 80
WiFiServer server(80);

// only runs once on boot
void setup() {
//  pinMode(LED, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,HIGH);
//  digitalWrite(LED, HIGH);
  
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  DS18B20.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}

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
  frequency_red = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency_red to the RGB Model of 0 to 255
  frequency_red = map(frequency_red, 25,72,255,0);
  // Printing the value on the serial monitor
  Serial.print("<table><tr><td style='background-color:rgb(");//printing name
  Serial.print(frequency_red); //printing RED color frequency
  Serial.print(",");
  delay(100);
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency_green = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency_green to the RGB Model of 0 to 255
  frequency_green = map(frequency_green, 30,90,255,0);
  // Printing the value on the serial monitor
  Serial.print("");//printing name
  Serial.print(frequency_green);//printing GREEN color frequency
  Serial.print(",");
  delay(100);
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency_blue
  frequency_blue = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency_blue to the RGB Model of 0 to 255
  frequency_blue = map(frequency_blue, 25,70,255,0);
  // Printing the value on the serial monitor
  Serial.print("");//printing name
  Serial.print(frequency_blue);//printing BLUE color frequency
  Serial.print(")'>..............................</td></tr></table>");
  delay(100);
}
// runs over and over again
void loop() {
  // Listenning for new clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (true) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && blank_line) {
            getTemperature();
            getfrequency();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            // your actual web page that displays temperature
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head></head><body><h1>ESP8266 - Temperature</h1><h3>Temperature in Celsius: ");
            client.println(temperatureCString);
            client.println("*C</h3><h3>Temperature in Fahrenheit: ");
            client.println(temperatureFString);
            client.println("*F</h3>");
            client.println("<h3>Color</h3>");
            client.println("<table><tr><td bgcolor='rgb(");
            client.print(frequency_red);
            client.print(",");
            client.print(frequency_green);
            client.print(",");
            client.print(frequency_blue);
            client.print(")'>..............................</td></tr></table>");
            client.println("</body></html>");  
            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}   
