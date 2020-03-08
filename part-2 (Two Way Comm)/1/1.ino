#include <ESP8266WiFi.h>
WiFiServer server(80);
const char* ssid = "delta2";
const char* password = "24081999";
IPAddress IP(192,168,4,15);
IPAddress mask = (255, 255, 255, 0);
byte ledPin = 2;
void setup() {
 Serial.begin(9600);
 Serial.print("Setting soft-AP ... ");
 WiFi.mode(WIFI_AP);
 WiFi.softAP(ssid, password);
 WiFi.softAPConfig(IP, IP, mask);
 server.begin();
 pinMode(ledPin, OUTPUT);
 Serial.println();
 Serial.println("accesspoint_bare_01.ino");
 Serial.println("Server started.");
 Serial.print("IP: "); Serial.println(WiFi.softAPIP());
 Serial.print("MAC:"); Serial.println(WiFi.softAPmacAddress());
}
void loop() {
 delay(2000);
 Serial.println("********************************");
 Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum()); 
 WiFiClient client = server.available();
 if (!client) {return;}
 digitalWrite(ledPin, LOW);
 String request = client.readStringUntil('\r');
 Serial.println("From the station: " + request);
 client.flush();
 String response;
 if(request=="DRY"){
  response="Water it!";
 }else if(request=="WET"){
  response="Don't water it!";
 }else{
  response="Invalid Request!";
 }
 Serial.print("Byte sent to the station: ");
 Serial.println(client.println(response+"\r"));
 digitalWrite(ledPin, HIGH);
 delay(2000);
}
