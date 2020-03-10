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
 String moisture = getValue(request, ',', 0);
 String temp = getValue(request, ',', 1);
 if(moisture=="DRY"){
  response="Water it!";
 }else if(moisture=="WET"){
  response="Don't water it!";
 }else{
  response="Invalid Request!";
 }
 Serial.print("Byte sent to the station: ");
 Serial.println(client.println(response+"\r"));
 digitalWrite(ledPin, HIGH);
 delay(2000);
}

String getValue(String data, char separator, int index){
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
