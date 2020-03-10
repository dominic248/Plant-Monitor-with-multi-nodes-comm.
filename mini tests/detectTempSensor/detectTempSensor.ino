#include  <OneWire.h >
OneWire ds(2);
void setup() {
 // put your setup code here, to run once:
 Serial.begin(115200);
}

void temperature(){
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
 return;
 }
 if ( ds.search(addr)) {
 Serial.println("fallo");
 }
 
 for( i = 0; i < 8; i++) { 
 addr[i];
 }
 if (OneWire::crc8(addr, 7) != addr[7]) {
 Serial.println("CRC is not valid!");
 return;
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
 
 return;
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
}

void loop() {
  // put your main code here, to run repeatedly:
 temperature(); 
 delay(1000);
 }
