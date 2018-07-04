#include <ESP8266WiFi.h>
#include <OneWire.h>
const char* ssid     = "router_it";
const char* password = "*";
const char* host = "165.227.163.172";

OneWire  ds(0); 

byte addr[8]; 
float temperature;


float getTemp(){
  byte data[12];  

  if (!ds.search(addr)) {
    Serial.println("No more addresses."); 
    while(1);
  }
   ds.reset_search(); 
 
  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      while(1);
  }

  
  ds.reset();            
  ds.select(addr);        
  ds.write(0x44);      
  delay(1000);   
  
  ds.reset();
  ds.select(addr);    
  ds.write(0xBE);          

  for (int i = 0; i < 9; i++) {           
    data[i] = ds.read();  
  }

  int raw = (data[1] << 8) | data[0]; 
  if (data[7] == 0x10) raw = (raw & 0xFFF0) + 12 - data[6];  
  return raw / 16.0;
    
} 


void setup() {
  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network
  Serial.println();
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
}

void loop() {

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }


 float temperature = getTemp();
 float fahrenheit = temperature * 1.8 + 32.0;
 Serial.println(temperature);
 //float tepm = temperature;
 char one_c1[10];
 char one_c2[10];
  dtostrf(temperature, 5, 2, one_c1);
  dtostrf(fahrenheit, 5, 2, one_c2);

 //String data = "sensor>MDEyMzQ1Njc4OTE="+"||value1>" + String(random(0,100)) +"||value2>" + String(random(0,100));
    
    String data = "param1=" + String(one_c1) +"&param2=" + String(one_c2) + "&sensor=MDEyMzQ1Njc4OTE=";
    
   Serial.print("Requesting POST: ");
   // Send request to the server:
   client.println("POST /sensor/new HTTP/1.1");
   client.println("Host: 165.227.163.172");
   client.println("Accept: */*");
   client.println("Content-Type: application/x-www-form-urlencoded");
   client.print("Content-Length: ");
   client.println(data.length());
   client.println();
   client.print(data);

   delay(500); // Can be changed
  if (client.connected()) { 
    client.stop();  // DISCONNECT FROM THE SERVER
  }
  Serial.println();
  Serial.println("closing connection");
  delay(5*60000);
}
