#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

extern "C" {
  #include<user_interface.h>
}

String moisture="",temp="";
WiFiServer server(80);
AsyncWebServer server1(8080);
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
  // Start server
  server1.begin();
}



void loop() {
 delay(2000);
 Serial.println("********************************");
 client_status();
 Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum()); 
 WiFiClient client = server.available();
 if (!client) {return;}
 digitalWrite(ledPin, LOW);
 String request = client.readStringUntil('\r');
 Serial.println("From the station: " + request);
 client.flush();
 String response;
 moisture = getValue(request, ',', 0);
 temp = getValue(request, ',', 1);
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
 server1.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain",moisture.c_str());
 });
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

void client_status() {
 
  unsigned char number_client;
  struct station_info *stat_info;
  
  struct ip_addr *IPaddress;
  IPAddress address;
  int i=1;
  
  number_client= wifi_softap_get_station_num();
  stat_info = wifi_softap_get_station_info();
  
  Serial.print(" Total Connected Clients are = ");
  Serial.println(number_client);
  
    while (stat_info != NULL) {
    
      ipv4_addr *IPaddress = &stat_info->ip;
      address = IPaddress->addr;
      
      Serial.print("client= ");
      
      Serial.print(i);
      Serial.print(" IP adress is = ");
      Serial.print((address));
      Serial.print(" with MAC adress is = ");
      String str = "Station #";
      for(i=0;i<6;i++){
        String val=String(stat_info->bssid[i], HEX);
        if(val.length()==1){
          str +=0;
        }
        str +=val;
        if(i<5){
          str += ":";
        }
      }
      str += "\r\n";
      Serial.print(str);
      stat_info = STAILQ_NEXT(stat_info, next);
      i++;
      Serial.println();
    }
}
