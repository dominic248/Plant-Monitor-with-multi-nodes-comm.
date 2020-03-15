#include <ESP8266WiFi.h>
#include  <OneWire.h >

const char* ssid = "delta2";
const char* password = "24081999";
byte ledPin = 2;
#define MOISTURE_ANALOG A0  /* Connect Soil moisture analog sensor pin to A0 of NodeMCU */
#define MOISTURE_DIGITAL D0
#define ds18b20 2
String type="",temp="";
OneWire ds(ds18b20);
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
  client.connect(server, 80);
  digitalWrite(ledPin, LOW);
  Serial.println("********************************");
//  type=readAnalogMoisture();
  type=readDigitalMoisture();
  temp=temperature();
  Serial.print("Byte sent to the AP: ");
//  Serial.println(client.print(String(moisture_percentage)+"\r"));
  Serial.println(client.print(type+","+temp+"\r"));
  String answer = client.readStringUntil('\r');
  Serial.println("From the AP: " + answer);
  client.flush();
  digitalWrite(ledPin, HIGH);
  client.stop();
  delay(2000);
}



String readDigitalMoisture() {
  int val=digitalRead(MOISTURE_DIGITAL);
  Serial.println("Soil Moisture = "+String(val));
  if (val) {
    return "DRY";
  } else {
    return "WET";
  }
}

String readAnalogMoisture(){
  float a0=analogRead(MOISTURE_ANALOG);
  float moisture_percentage = ( 100.00 - ( (a0/1023.00) * 100.00 ) );
  a0= a0/10;
  Serial.println("Soil Moisture = "+String(a0));
  if(a0<50){
    return "WET";
  }else{
    return "DRY";
  }
}

String temperature(){
 byte i;
 byte present = 0;
 byte type_s;
 byte data[12];
 byte addr[8];
 float celsius, fahrenheit;
 
 if ( !ds.search(addr)) {
 /// Serial.println("No more addresses.");
 /// Serial.println();
 ds.reset_search();
 delay(250);
 return String(0);;
 }
 if ( ds.search(addr)) {
 Serial.println("fallo");
 }
 
 for( i = 0; i < 8; i++) { 
 addr[i];
 }
 if (OneWire::crc8(addr, 7) != addr[7]) {
 Serial.println("CRC is not valid!");
 return String(0);
 }
 
 // the first ROM byte indicates which chip
 switch (addr[0]) {
 case 0x10:

 type_s = 1;
 break;
 case 0x28:

 type_s = 0;
 break;
 case 0x22:
 // Serial.println(" Chip = DS1822");
 type_s = 0;
 break;
 default:
 Serial.println("Device is not a DS18x20 family device.");
 
 return String(0);
 } 

 ds.reset();
 ds.select(addr);
 ds.write(0x44, 1); 
  delay(1000); 

 present = ds.reset();
 ds.select(addr); 
 ds.write(0xBE); 

 //Serial.println("Paso 7");
 
 for ( i = 0; i < 9; i++) { 
 data[i] = ds.read();
 }
 
 OneWire::crc8(data, 8); 

 int16_t raw = (data[1] << 8) | data[0];
 if (type_s) {
 raw = raw << 3; 
 if (data[7] == 0x10) { 
 raw = (raw & 0xFFF0) + 12 - data[6]; }
 } else {
 byte cfg = (data[4] & 0x60);

 if (cfg == 0x00) raw = raw & ~7; // 9 bit resolution, 93.75 ms
 else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
 else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }
 
 float celsius_web; 
 celsius = (float)raw / 16.0;

 if (isnan(celsius)) {
 Serial.println("isnan");
 }

 Serial.println("Temperature: "+String(celsius));
 return String(celsius);
}
