#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

extern "C" {
  #include<user_interface.h>
}

#define ARRAYSIZE 10
String results[ARRAYSIZE] = { "192.168.4.114" };
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
  server1.on("/page", HTTP_GET, handleRoot);
  server1.on("/login", HTTP_POST, handleLogin);
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
  for (int i =0; i< ARRAYSIZE; i++) {
    if(results[i]==ipaddr){
      response="water it!";
      break;
    }
  }
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

void client_status() {
  unsigned char number_client;
  struct station_info *stat_info;
  struct ip_addr *IPaddress;
  IPAddress address;
  int i=1;
  number_client= wifi_softap_get_station_num();
  stat_info = wifi_softap_get_station_info();
  Serial.print("Total Connected Clients are = ");
  Serial.println(number_client);
    while (stat_info != NULL) {
      ipv4_addr *IPaddress = &stat_info->ip;
      address = IPaddress->addr;
      Serial.print("client = ");
      Serial.print(i);
      Serial.print(" IP adress is = ");
      Serial.print((address));
      Serial.print(" with MAC adress is = ");
      String str = "";
      for(i=0;i<6;i++){
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




void handleRoot() {                          // When URI / is requested, make login Webpage
  server1.send(200, "text/html", "<form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"uname\" placeholder=\"Username\"></br><input type=\"password\" name=\"pass\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form><p>Try 'User1' and 'Pass1' ...</p>");
}
void handleLogin() {                         //Handle POST Request
  if( ! server1.hasArg("uname") || ! server1.hasArg("pass") 
      || server1.arg("uname") == NULL || server1.arg("pass") == NULL) { // Request without data
    server1.send(400, "text/plain", "400: Invalid Request");         // Print Data on screen
    return;
  }
  if(server1.arg("uname") == "User1" && server1.arg("pass") == "Pass1") { // If username and the password are correct
    server1.send(200, "text/html", "<h1>Hello, " + server1.arg("uname") + "!</h1><p>Login successful</p>");
  } else {                                                                              // Username and password don't match
    server1.send(401, "text/plain", "401: Invalid Credentials");
  }
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
