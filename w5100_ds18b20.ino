/*
   Web client sketch for IDE v1.0.1 and w5100/w5200
   Uses POST method.
   Posted November 2012 by SurferTim
*/

#include <SPI.h>
#include <Ethernet.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//Change to your server domain
char serverName[] = "165.227.163.172";

// change to your server's port
int serverPort = 80;

// change to the page on that server
char pageName[] = "/sensor/new";


String sensor = "MDEyMzQ1Njc4OTI=";
EthernetClient client;
int totalCount = 0; 
// insure params is big enough to hold your variables
char params[64];

// set this to the number of milliseconds delay
// this is 60 seconds
#define delayMillis 5*60000UL

unsigned long thisMillis = 0;
unsigned long lastMillis = 0;

DeviceAddress Thermometer1; // для любого, которий есть на шыне

void setup() {
  Serial.begin(9600);

  // disable SD SPI
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  Serial.print(F("Starting ethernet..."));
  if(!Ethernet.begin(mac)) Serial.println(F("failed"));
  else Serial.println(Ethernet.localIP());

  delay(2000);
  Serial.println(F("Ready"));
  sensors.begin();
  sensors.getAddress(Thermometer1, 0);//получить адрес №1
  sensors.setResolution(Thermometer1, 10);
      sensors.requestTemperatures(); 
    // params must be url encoded.
    float t = sensors.getTempCByIndex(0);
    float h = sensors.getTempFByIndex(0);
    Serial.println();
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    
    char str_temp1[6];
    char str_temp2[6];
    /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
    dtostrf(t, 5, 2, str_temp1);
    dtostrf(h, 5, 2, str_temp2);

    String PostData="sensor="+sensor+"&param1="+String(str_temp1)+"&param2="+String(str_temp2);
    PostData.toCharArray(params, 64);
    
//    sprintf(params,"{t:%s,h:%s}",str_temp1,str_temp2);     
    
    if(!postPage(serverName,serverPort,pageName,params)) Serial.print(F("Fail "));
    else Serial.print(F("Pass "));
  
}

void loop()
{
  // If using a static IP, comment out the next line
  Ethernet.maintain();

  thisMillis = millis();

  if(thisMillis - lastMillis > delayMillis)
  {
    lastMillis = thisMillis;
    sensors.requestTemperatures(); 
    // params must be url encoded.
    float t = sensors.getTempCByIndex(0);
    float h = sensors.getTempFByIndex(0);
    Serial.println();
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    
    char str_temp1[6];
    char str_temp2[6];
    /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
    dtostrf(t, 5, 2, str_temp1);
    dtostrf(h, 5, 2, str_temp2);

    String PostData="sensor="+sensor+"&param1="+String(str_temp1)+"&param2="+String(str_temp2);
    PostData.toCharArray(params, 64);
    
//    sprintf(params,"{t:%s,h:%s}",str_temp1,str_temp2);     
    
    if(!postPage(serverName,serverPort,pageName,params)) Serial.print(F("Fail "));
    else Serial.print(F("Pass "));
    totalCount++;
    Serial.println(totalCount,DEC);
  }    
}


byte postPage(char* domainBuffer,int thisPort,char* page,char* thisData)
{
  int inChar;
  char outBuf[64];

  Serial.print(F("connecting..."));

  if(client.connect(domainBuffer,thisPort) == 1)
  {
    Serial.println(F("connected"));

    // send the header
    sprintf(outBuf,"POST %s HTTP/1.1",page);
    client.println(outBuf);
    sprintf(outBuf,"Host: %s",domainBuffer);
    client.println(outBuf);
    client.println(F("Connection: close\r\nContent-Type: application/x-www-form-urlencoded"));
    sprintf(outBuf,"Content-Length: %u\r\n",strlen(thisData));
    client.println(outBuf);

    // send the body (variables)
    client.print(thisData);
  } 
  else
  {
    Serial.println(F("failed"));
    return 0;
  }

  int connectLoop = 0;

  while(client.connected())
  {
    while(client.available())
    {
      inChar = client.read();
      Serial.write(inChar);
      connectLoop = 0;
    }

    delay(1);
    connectLoop++;
    if(connectLoop > 10000)
    {
      Serial.println();
      Serial.println(F("Timeout"));
      client.stop();
    }
  }

  Serial.println();
  Serial.println(F("disconnecting."));
  client.stop();
  return 1;
}
