#include <Arduino.h>
#include "ESPBASE.h"
#include "OneWire.h"
#include "DallasTemperature.h"

OneWire oneWire(4);
DallasTemperature DS18B20(&oneWire);

WiFiClient espClient;
const byte mqttDebug = 1;
int numberOfDevices; //Number of temperature devices found
long lastTemp; //The last measurement
DeviceAddress devAddr[15];  //An array device temperature sensors
const int durationTemp = 5000; //The frequency of temperature measurement
float tempDev[15]; //Saving the last measurement of temperature
float tempDevLast[15]; //Previous temperature measurement
int relay1state = 0;
int relay2state = 0;
#define RELAY1_PIN 12
#define RELAY2_PIN 13
long lastVolts = 0;
long durationVolts = 10000;

ESPBASE Esp;

//------------------------------------------
//Convert device id to String
String GetAddressToString(DeviceAddress deviceAddress){
  String str = "";
  for (uint8_t i = 0; i < 8; i++){
    if( deviceAddress[i] < 16 ) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
  }
  return str;
}

//Setting the temperature sensor
void SetupDS18B20(){
//  Serial.begin(115200);
  DS18B20.begin();

  Serial.print("Parasite power is: "); 
  if( DS18B20.isParasitePowerMode() ){ 
    Serial.println("ON");
  }else{
    Serial.println("OFF");
  }
  
  numberOfDevices = DS18B20.getDeviceCount();
  Serial.print( "Device count: " );
  Serial.println( numberOfDevices );

  lastTemp = millis();
  DS18B20.requestTemperatures();

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if( DS18B20.getAddress(devAddr[i], i) ){
      //devAddr[i] = tempDeviceAddress;
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: " + GetAddressToString(devAddr[i]));
      Serial.println();
    }else{
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }

    //Get resolution of DS18b20
    Serial.print("Resolution: ");
    Serial.print(DS18B20.getResolution( devAddr[i] ));
    Serial.println();

    //Read temperature from DS18b20
    float tempC = DS18B20.getTempC( devAddr[i] );
    Serial.print("Temp C: ");
    Serial.println(tempC);
  }
}

void setup() 
{
  Serial.begin(115200);
  pinMode(RELAY1_PIN,OUTPUT);
  pinMode(RELAY2_PIN,OUTPUT);
  SetupDS18B20();
  Esp.initialize();
}

void VoltLoop()
{
  long t=millis();
  if(t - lastVolts > durationVolts)
  {
    Serial.println("Reading volts");
    int a = analogRead(A0);
    Serial.println(a);
    lastVolts = t;
    float v=(1/1024.0*(float)a);
    float tmp = 34000.0/(150000.0+34000.0);
    float v2=v/tmp;
    Esp.mqttSend(String(DEVICE_TYPE) + "/" + config.DeviceName + "/value","volts:" + String(v2),"");
  }
}

void handleTemp(int sensor,float tempF)
{
  int itemp = (int) tempF;
  if(itemp > 150 || itemp < -50)
    return;
  
  if(sensor == config.rly1sensor)
  {
    if(config.rly1ontemp > config.rly1offtemp)
    {
      if(itemp >= config.rly1ontemp)
      {
        relay1state = 1;
        digitalWrite(RELAY1_PIN,HIGH);
      }
      if(itemp <= config.rly1offtemp)
      {
        relay1state = 0;
        digitalWrite(RELAY1_PIN,LOW);
      }
    }
    else
    {
      if(itemp <= config.rly1ontemp)
      {
        relay1state = 1;
        digitalWrite(RELAY1_PIN,HIGH);
      }
      if(itemp >= config.rly1offtemp)
      {
        relay1state = 0;
        digitalWrite(RELAY1_PIN,LOW);
      }
    }
  }
  if(sensor == config.rly2sensor)
  {
    if(config.rly2ontemp > config.rly2offtemp)
    {
      if(itemp >= config.rly2ontemp)
      {
        relay2state = 1;
        digitalWrite(RELAY2_PIN,HIGH);
      }
      if(itemp <= config.rly1offtemp)
      {
        relay2state = 0;
        digitalWrite(RELAY2_PIN,LOW);
      }
    }
    else
    {
      if(itemp <= config.rly2ontemp)
      {
        relay2state = 1;
        digitalWrite(RELAY2_PIN,HIGH);
      }
      if(itemp >= config.rly2offtemp)
      {
        relay2state = 0;
        digitalWrite(RELAY2_PIN,LOW);
      }
    }
  }
}

//Loop measuring the temperature
void TempLoop(long now)
{
  String msgline;
  if( now - lastTemp > durationTemp )
  { //Take a measurement at a fixed time (durationTemp = 5000ms, 5s)
//    Serial.println(numberOfDevices);
    if(numberOfDevices == 0)
    {
      Serial.print("reset = ");Serial.println(oneWire.reset());
      DeviceAddress deviceAddress;

      oneWire.reset_search();
      while (oneWire.search(deviceAddress)) 
      {
        Serial.println("got address");
      }
      DS18B20.begin();
      numberOfDevices = DS18B20.getDeviceCount();
      
      Serial.print( "Device count: " );
      Serial.println( numberOfDevices );
    }
    msgline = "";
    for(int i=0; i<numberOfDevices; i++)
    {
      float tempF = DS18B20.getTempF( devAddr[i] ); //Measuring temperature in Celsius
      tempDev[i] = tempF; //Save the measured value to the array
//      msgline = msgline + String(i) + "| Temp F: " + String(tempF) + " ";
      handleTemp(i,tempF);
      Esp.mqttSend(String(DEVICE_TYPE) + "/" + config.DeviceName + "/value","",String(i) + ":" + String(tempF)); 
    }
//    msgline = msgline + "| relay 1 = " + String(relay1state) + " | relay 2 = " + String(relay2state);
    Serial.println(msgline);
    DS18B20.setWaitForConversion(false); //No waiting for measurement
    DS18B20.requestTemperatures(); //Initiate the temperature measurement
    lastTemp = millis();  //Remember the last time measurement
  }
}

void loop() {
  //  WebServer requests handling
  Esp.loop();
  long t=millis();
//  TempLoop(t);
  VoltLoop();
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char c_payload[length];
  memcpy(c_payload, payload, length);
  c_payload[length] = '\0';
  
  String s_topic = String(topic);
  String s_payload = String(c_payload);
  
  if (mqttDebug) {
    Serial.print("MQTT in: ");
    Serial.print(s_topic);
    Serial.print(" = ");
    Serial.print(s_payload);
  }

  if (s_topic == "TestTopic") {
    Serial.println("Got set: ");
    Serial.println(s_payload);
    char dname[20];
    String DeviceName;
    char *p = c_payload;
    char *str;
    str = strtok_r(p,";",&p);
    strncpy(dname,str,20);
    Serial.print("device name: ");
    DeviceName = String(dname);
    Serial.println(DeviceName);
    if(DeviceName == String(config.DeviceName))
    {
      Serial.println("This is for me");
    }
  }
  else {
    if (mqttDebug) { Serial.println(" [unknown message]"); }
  }
}

void mqttSubscribe()
{
    if (Esp.mqttClient->connected()) {
      //subscribe to topics here
    }
}

