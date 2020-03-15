#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

extern "C" {
  #include<user_interface.h>
}

#define MAIN_ARRAYSIZE 50
int main_count=0;
String main_ips[MAIN_ARRAYSIZE] = { "0" };
#define WATER_ARRAYSIZE 50
int water_count=0;
String water_ips[WATER_ARRAYSIZE] = { "0" };
String moisture="",temp="",ipaddr="";
String response="Don't water it!";
bool manual=false;
WiFiServer server(80);
ESP8266WebServer server1(8080);
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

  server1.on("/",HTTP_GET, handle_OnConnect);
  server1.on("/addwater", HTTP_GET, handleWater);
  server1.on("/addwaterit", HTTP_POST, handleWaterIt);
  server1.on("/removewater", HTTP_GET, handleRemoveWater);
  server1.on("/removewaterit", HTTP_POST, handleRemoveWaterIt);
  server1.on("/manualon",HTTP_GET, handle_manualon);
  server1.on("/manualoff",HTTP_GET, handle_manualoff);
  server1.onNotFound(handle_NotFound);
  server1.begin();
}



void loop() {
 delay(2000);
 server1.handleClient();
 
 Serial.println("********************************");
 Serial.println("Manual Mode = "+String(manual));
 Serial.print("All main IPs: ");
 for(int i=0;i<water_count;i++){
    Serial.print(water_ips[i]+", ");
 }Serial.println();
 Serial.print("All water IPs: ");
 for(int i=0;i<main_count;i++){
    Serial.print(main_ips[i]+" ");
 }Serial.println();
 client_status();
 WiFiClient client = server.available();
 if (!client) {return;}
 digitalWrite(ledPin, LOW);
 String request = client.readStringUntil('\r');
 Serial.println("From the station: " + request);
 client.flush();
 
 moisture = getValue(request, ',', 0);
 temp = getValue(request, ',', 1);
 ipaddr = getValue(request, ',', 2);
 if(!manual){
   if(moisture=="DRY"){
    response="Water it!";
   }else if(moisture=="WET"){
    response="Don't water it!";
   }else{
    response="Invalid Request!";
   }
 }
 if(manual){
  int check=0;
  for (int i =0; i< water_count; i++) {
    Serial.println(water_ips[i]);
    if(water_ips[i]==ipaddr){
      check=1;
      response="water it!";
      break;
    }
  }
  if(check==0){
    response="dont water it!";
  }
 }
 Serial.print("Byte sent to the station: ");
 Serial.println(client.println(response+"\r"));
 digitalWrite(ledPin, HIGH);

 delay(2000);
}

void deleteElement(String arr[], int n, String x) { 
  int i;
   for (i=0; i<n; i++) 
      if (arr[i] == x) 
         break; 
   if (i < n) { 
       n = n - 1; 
       for (int j=i; j<n; j++) 
          arr[j] = arr[j+1]; 
   } 
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
  Serial.print("Total Connected Clients are = ");
  main_count=0;
  Serial.println(number_client);
    while (stat_info != NULL) {
      ipv4_addr *IPaddress = &stat_info->ip;
      address = IPaddress->addr;
      Serial.print("client = ");
      Serial.print(i);
      Serial.print(" IP adress is = ");
      Serial.print((address));
      main_ips[main_count]=address.toString();
      main_count++;
      Serial.print(" with MAC adress is = ");
      String str = "";
      for(int i=0;i<6;i++){
        String val=String(stat_info->bssid[i], HEX);
        if(val.length()==1){str +=0;}
        str +=val;
        if(i<5){str += ":";}
      }
      str += "\r\n";
      Serial.print(str);
      stat_info = STAILQ_NEXT(stat_info, next);
      i++;
    }
}






void handleWater() {   
  String str="";                       
  for(int i=0;i<main_count;i++){
    str+="<input type=\"radio\" name=\"water\" value="+main_ips[i]+" />"+main_ips[i]+"<br />";
  }
  server1.send(200, "text/html", "<form action=\"/addwaterit\" method=\"POST\">"+str+"<input type=\"submit\" value=\"Start Water\"></form>");
}

void handleWaterIt() {                         //Handle POST Request
  water_ips[water_count]=String(server1.arg("water"));
  water_count++;
  server1.send(200, "text/html", "done");
}

void handleRemoveWater() {   
  String str="";                       
  for(int i=0;i<water_count;i++){
    str+="<input type=\"radio\" name=\"water\" value="+water_ips[i]+" />"+water_ips[i]+"<br />";
  }
  server1.send(200, "text/html", "<form action=\"/removewaterit\" method=\"POST\">"+str+"<input type=\"submit\" value=\"Stop Water\"></form>");
}

void handleRemoveWaterIt() {                         //Handle POST Request
  deleteElement(water_ips, water_count, String(server1.arg("water")));
  water_count--;
  server1.send(200, "text/html", "done");
}

 




void handle_OnConnect() {
  server1.send(200, "text/html", SendHTML(manual)); 
}

void handle_manualon() {
  manual = true;
  server1.sendHeader("Location", "/",true);
  server1.send(302, "text/html", SendHTML(true)); 
}

void handle_manualoff() {
  manual = false;
  server1.sendHeader("Location", "/",true);
  server1.send(302, "text/html", SendHTML(false)); 
}

void handle_NotFound(){
  server1.sendHeader("Location", "/",true);
  server1.send(302, "text/plain", SendHTML(manual));
}

String SendHTML(uint8_t  manual1){
  String ptr = "<!DOCTYPE html>\n";
  ptr +="<html>\n";
  ptr +="<head>\n";
  ptr +="<title>Manual Control</title>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Manual Mode</h1>\n";
  ptr +="<p>Click to switch <b>Manual Mode</b> on and off.</p>\n";
  ptr +="<form method=\"get\">\n";
  if(manual1)
  ptr +="<input type=\"button\" value=\"manual OFF\" onclick=\"window.location.href='/manualoff'\">\n";
  else
  ptr +="<input type=\"button\" value=\"manual ON\" onclick=\"window.location.href='/manualon'\">\n";
  ptr +="</form>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
