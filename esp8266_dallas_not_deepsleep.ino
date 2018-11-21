#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <DallasTemperature.h>


const char* host = "165.227.163.172";


// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS D3

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

WiFiClient client;

long previousMillis = 60000;        // храним время последнего переключения 
long interval = 1000*60*5;          // интервал между проверкой доступности сервера (300 секунд)

long interval_restart = 1000*60*60; // интервал между включение/выключением (60 минут)

long interval_blink = 1000;         // интервал блинка
long previousMillis_blink = 1000;   // храним время последнего переключения 
int ledState = LOW;     

void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(100);
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");

  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  //WiFi.begin(ssid, password);
  //int start = 0;
 //while (WiFi.status() != WL_CONNECTED) {
 //   delay(500);
 //   Serial.print(".");
 // }

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP()); 

  
 
}

void PostData()
{

  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
 //   ESP.deepSleep(60e6); // 300e6 is 5 mimute
    return;
  }
 
 sensors.begin();
 delay(1000);
 sensors.requestTemperatures(); // Send the command to get temperatures
 

 float temperature = sensors.getTempCByIndex(0);
 float fahrenheit = temperature * 1.8 + 32.0;
 Serial.println(temperature);
 char one_c1[10];
 char one_c2[10];
  dtostrf(temperature, 5, 2, one_c1);
  dtostrf(fahrenheit, 5, 2, one_c2);

 
    
    String data = "param1=" + String(one_c1) +"&param2=" + String(one_c2) + "&sensor=MTIzNDU1Njg3OQ==";
    
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
//  ESP.deepSleep(30e6); // 300e6 is 5 mimute
}


void loop() {
  
 unsigned long currentMillis = millis();
 
    //проверяем не прошел ли нужный интервал, если прошел то
  if(currentMillis - previousMillis > interval) {
    // сохраняем время последнего переключения
    previousMillis = currentMillis; 
    PostData();
  }
   if(currentMillis > interval_restart) {
      //Перезапускаем ESP 
      ESP.restart();
   }

    if(currentMillis - previousMillis_blink > interval_blink) {
      //Перезапускаем ESP 
     if (ledState == LOW)
      ledState = HIGH;  // Note that this switches the LED *off*
    else
      ledState = LOW;   // Note that this switches the LED *on*
    
      digitalWrite(LED_BUILTIN, ledState); 
      previousMillis_blink = currentMillis;   
   }
}
