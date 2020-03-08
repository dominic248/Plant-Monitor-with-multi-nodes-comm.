#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

#include <Wire.h>


// Set your access point network credentials
const char* ssid = "delta2";
const char* password = "";
int sensor_pin = A0;
int output_value ;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);
  Serial.println();
  
  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  //Soil moisture
  

server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
  output_value= analogRead(sensor_pin);
  Serial.print("Moisture : ");
  Serial.print(output_value);
   Serial.println("%");
    request->send_P(200, "text/plain", String(output_value).c_str());
  });
  
  // Start server
  server.begin();
}
 
void loop(){
  
}
