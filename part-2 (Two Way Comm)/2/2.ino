#include <ESP8266WiFi.h>
const char* ssid = "delta2";
const char* password = "24081999";
byte ledPin = 2;
const int sensor_pin = A0;  /* Connect Soil moisture analog sensor pin to A0 of NodeMCU */

IPAddress server(192,168,4,15);     // IP address of the AP
WiFiClient client;
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);           // connects to the WiFi AP
  Serial.println();
  Serial.println("Connection to the AP");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected");
  Serial.println("station_bare_01.ino");
  Serial.print("LocalIP:"); Serial.println(WiFi.localIP());
  Serial.println("MAC:" + WiFi.macAddress());
  Serial.print("Gateway:"); Serial.println(WiFi.gatewayIP());
  Serial.print("AP MAC:"); Serial.println(WiFi.BSSIDstr());
  pinMode(ledPin, OUTPUT);
}
void loop() {
  float a0=analogRead(sensor_pin);
  float moisture_percentage = ( 100.00 - ( (a0/1023.00) * 100.00 ) );
  String type;
  a0= a0/10;
  if(a0<50){
    type="WET";
  }else{
    type="DRY";
  }
  client.connect(server, 80);
  digitalWrite(ledPin, LOW);
  Serial.println("********************************");
  Serial.print("Byte sent to the AP: ");
//  Serial.println(client.print(String(moisture_percentage)+"\r"));
  Serial.println(client.print(type+"\r"));
  Serial.println("Soil Moisture= "+String(a0));
  String answer = client.readStringUntil('\r');
  Serial.println("From the AP: " + answer);
  client.flush();
  digitalWrite(ledPin, HIGH);
  client.stop();
  delay(2000);
}
