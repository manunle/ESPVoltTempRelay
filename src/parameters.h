#ifndef PARAMETERS_H
#define PARAMETERS_H
#define CFGVER "1.4"

struct strConfig {
  boolean dhcp;                         // 1 Byte - EEPROM 4
  boolean isDayLightSaving;             // 1 Byte - EEPROM 5
  long Update_Time_Via_NTP_Every;       // 4 Byte - EEPROM 6
  long timeZone;                        // 4 Byte - EEPROM 10
  byte  IP[4];                          // 4 Byte - EEPROM 14
  byte  Netmask[4];                     // 4 Byte - EEPROM 18
  byte  Gateway[4];                     // 4 Byte - EEPROM 22
  String ssid;                          // up to 32 Byte - EEPROM 26
  String password;                      // up to 32 Byte - EEPROM 58
  String ntpServerName;                 // up to 32 Byte - EEPROM 90
  String DeviceName;                    // up to 32 Byte - EEPROM 122
  String OTApwd;                         // up to 32 Byte - EEPROM 154
  String MQTTServer;                    // up to 32 Byte - EEPROM 186
  long MQTTPort;                        // 4 Byte - EEPROM 218
  String HeartbeatTopic;                // up to 32 Byte - EEPROM 222
  long HeartbeatEvery;                  // 4 Byte - EEPROM 254
  int rly1sensor;                       // 2 Bytes - EEPROM 258
  int rly1ontemp;                       // 2 Bytes - EEPROM 260
  int rly1offtemp;                      // 2 Bytes - EEPROM 262
  int rly2sensor;                       // 2 Bytes - EEPROM 264
  int rly2ontemp;                       // 2 Bytes - EEPROM 266
  int rly2offtemp;                      // 2 Bytes - EEPROM 268
  // Application Settings here... from EEPROM 296 up to 511 (0 - 511)

} config;



#ifdef ARDUINO_ESP32_DEV

  #include <Preferences.h>

  Preferences EEPROM;

  void WriteConfig(){

    Serial.println("Writing Config");
    EEPROM.putString("head", CFGVER);


    EEPROM.putUChar("dhcp", config.dhcp);
    EEPROM.putUChar("isDayLight", config.isDayLightSaving);

    EEPROM.putULong("NTPrate", config.Update_Time_Via_NTP_Every); // 4 Byte
    EEPROM.putULong("timeZone", config.timeZone); // 4 Byte

    EEPROM.putUChar("ip0", config.IP[0]);
    EEPROM.putUChar("ip1", config.IP[1]);
    EEPROM.putUChar("ip2", config.IP[2]);
    EEPROM.putUChar("ip3", config.IP[3]);

    EEPROM.putUChar("nm0", config.Netmask[0]);
    EEPROM.putUChar("nm1", config.Netmask[1]);
    EEPROM.putUChar("nm2", config.Netmask[2]);
    EEPROM.putUChar("nm3", config.Netmask[3]);

    EEPROM.putUChar("gw0", config.Gateway[0]);
    EEPROM.putUChar("gw1", config.Gateway[1]);
    EEPROM.putUChar("gw2", config.Gateway[2]);
    EEPROM.putUChar("gw3", config.Gateway[3]);

    EEPROM.putString("ssid", config.ssid);
    EEPROM.putString("pwd", config.password);
    EEPROM.putString("ntpSN", config.ntpServerName);
    EEPROM.putString("DevN", config.DeviceName);
    EEPROM.putString("OTApwd", config.OTApwd);
    EEPROM.putString("MQTTServer",config.MQTTServer);
    EEPROM.putULong("MQTTPort",config.MQTTPort);
  }
  boolean ReadConfig(){
    Serial.println("Reading Configuration");
    if (EEPROM.getString("head") == CFGVER )
    {
      Serial.println("Configurarion Found!" + CFGVER);
      config.dhcp = EEPROM.getUChar("dhcp",0);
      config.isDayLightSaving = EEPROM.getUChar("isDayLight");
      config.Update_Time_Via_NTP_Every = EEPROM.getULong("NTPrate"); // 4 Byte
      config.timeZone = EEPROM.getULong("timeZone"); // 4 Byte
      config.IP[0] = EEPROM.getUChar("ip0");
      config.IP[1] = EEPROM.getUChar("ip1");
      config.IP[2] = EEPROM.getUChar("ip2");
      config.IP[3] = EEPROM.getUChar("ip3");
      config.Netmask[0] = EEPROM.getUChar("nm0");
      config.Netmask[1] = EEPROM.getUChar("nm1");
      config.Netmask[2] = EEPROM.getUChar("nm2");
      config.Netmask[3] = EEPROM.getUChar("nm3");
      config.Gateway[0] = EEPROM.getUChar("gw0");
      config.Gateway[1] = EEPROM.getUChar("gw1");
      config.Gateway[2] = EEPROM.getUChar("gw2");
      config.Gateway[3] = EEPROM.getUChar("gw3");
      config.ssid = EEPROM.getString("ssid");
      config.password = EEPROM.getString("pwd");
      config.ntpServerName = EEPROM.getString("ntpSN");
      config.DeviceName = EEPROM.getString("DevN");
      config.OTApwd = EEPROM.getString("OTApwd");
      config.MQTTServer = EEPROM.getString("MQTTServer");
      config.MQTTPort = EEPROM.getString("MQTTPort");
      // Application parameters here ... from EEPROM 192 to 511

      return true;

    }
    else
    {
      Serial.println("Configurarion NOT FOUND!!!!");
      return false;
    }
  }



#elif ARDUINO_ESP8266_ESP01 || ARDUINO_ESP8266_NODEMCU
  //  Auxiliar function to handle EEPROM
  // EEPROM-parameters
  void EEPROMWriteint(int address, int value){
    byte two = (value & 0xFF);
    byte one = ((value >> 8) & 0xFF);

    //Write the 2 bytes into the eeprom memory.
    EEPROM.write(address, two);
    EEPROM.write(address + 1, one);
  }

  int EEPROMReadint(int address){
    //Read the 2 bytes from the eeprom memory.
    int two = EEPROM.read(address);
    int one = EEPROM.read(address + 1);

    //Return the recomposed long by using bitshift.
    return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
  }

  void EEPROMWritelong(int address, long value){
    byte four = (value & 0xFF);
    byte three = ((value >> 8) & 0xFF);
    byte two = ((value >> 16) & 0xFF);
    byte one = ((value >> 24) & 0xFF);

    //Write the 4 bytes into the eeprom memory.
    EEPROM.write(address, four);
    EEPROM.write(address + 1, three);
    EEPROM.write(address + 2, two);
    EEPROM.write(address + 3, one);
  }

  long EEPROMReadlong(long address){
    //Read the 4 bytes from the eeprom memory.
    long four = EEPROM.read(address);
    long three = EEPROM.read(address + 1);
    long two = EEPROM.read(address + 2);
    long one = EEPROM.read(address + 3);

    //Return the recomposed long by using bitshift.
    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
  }

  void WriteStringToEEPROM(int beginaddress, String string){
    char  charBuf[string.length() + 1];
    string.toCharArray(charBuf, string.length() + 1);
    for (int t =  0; t < sizeof(charBuf); t++)
    {
      EEPROM.write(beginaddress + t, charBuf[t]);
    }
  }

  String  ReadStringFromEEPROM(int beginaddress){
    volatile byte counter = 0;
    char rChar;
    String retString = "";
    while (1)
    {
      rChar = EEPROM.read(beginaddress + counter);
      if (rChar == 0) break;
      if (counter > 31) break;
      counter++;
      retString.concat(rChar);

    }
    return retString;
  }

  void WriteConfig(){
    String cfgver = CFGVER;
    char ccfgver[4];
    cfgver.toCharArray(ccfgver,4);

    Serial.print("Writing Config ");
    Serial.println(ccfgver);
    EEPROM.write(0, ccfgver[0]);
    EEPROM.write(1, ccfgver[1]);
    EEPROM.write(2, ccfgver[2]);

    EEPROM.write(4, config.dhcp);
    EEPROM.write(5, config.isDayLightSaving);

    EEPROMWritelong(6, config.Update_Time_Via_NTP_Every); // 4 Byte
    EEPROMWritelong(10, config.timeZone); // 4 Byte

    EEPROM.write(14, config.IP[0]);
    EEPROM.write(15, config.IP[1]);
    EEPROM.write(16, config.IP[2]);
    EEPROM.write(17, config.IP[3]);

    EEPROM.write(18, config.Netmask[0]);
    EEPROM.write(19, config.Netmask[1]);
    EEPROM.write(20, config.Netmask[2]);
    EEPROM.write(21, config.Netmask[3]);

    EEPROM.write(22, config.Gateway[0]);
    EEPROM.write(23, config.Gateway[1]);
    EEPROM.write(24, config.Gateway[2]);
    EEPROM.write(25, config.Gateway[3]);

    WriteStringToEEPROM(26, config.ssid);
    WriteStringToEEPROM(58, config.password);
    WriteStringToEEPROM(90, config.ntpServerName);
    WriteStringToEEPROM(122, config.DeviceName);
    WriteStringToEEPROM(154, config.OTApwd);
    WriteStringToEEPROM(186, config.MQTTServer);
    EEPROMWritelong(218, config.MQTTPort);
    WriteStringToEEPROM(222,config.HeartbeatTopic);                // up to 32 Byte - EEPROM 260
    EEPROMWritelong(254,config.HeartbeatEvery);                  // 4 Byte - EEPROM 292
    EEPROMWriteint(258,config.rly1sensor);                       // 2 Bytes - EEPROM 258
    EEPROMWriteint(260,config.rly1ontemp);                       // 2 Bytes - EEPROM 260
    EEPROMWriteint(262,config.rly1offtemp);                      // 2 Bytes - EEPROM 262
    EEPROMWriteint(264,config.rly2sensor);                       // 2 Bytes - EEPROM 264
    EEPROMWriteint(266,config.rly2ontemp);                       // 2 Bytes - EEPROM 268
    EEPROMWriteint(268,config.rly2offtemp);                      // 2 Bytes - EEPROM 270

      // Application Settings here... from EEPROM 192 up to 511 (0 - 511)

    EEPROM.commit();

  }

  boolean ReadConfig(){
    Serial.println("Reading Configuration");
    String cfgver = CFGVER;
    char ccfgver[4];
    cfgver.toCharArray(ccfgver,4);
    if (EEPROM.read(0) == cfgver[0] && EEPROM.read(1) == cfgver[1]  && EEPROM.read(2) == cfgver[2] )
    {
      Serial.println("Configurarion Found!");
      config.dhcp = 	EEPROM.read(4);
      config.isDayLightSaving = EEPROM.read(5);
      config.Update_Time_Via_NTP_Every = EEPROMReadlong(6); // 4 Byte
      config.timeZone = EEPROMReadlong(10); // 4 Byte
      config.IP[0] = EEPROM.read(14);
      config.IP[1] = EEPROM.read(15);
      config.IP[2] = EEPROM.read(16);
      config.IP[3] = EEPROM.read(17);
      config.Netmask[0] = EEPROM.read(18);
      config.Netmask[1] = EEPROM.read(19);
      config.Netmask[2] = EEPROM.read(20);
      config.Netmask[3] = EEPROM.read(21);
      config.Gateway[0] = EEPROM.read(22);
      config.Gateway[1] = EEPROM.read(23);
      config.Gateway[2] = EEPROM.read(24);
      config.Gateway[3] = EEPROM.read(26);
      config.ssid = ReadStringFromEEPROM(26);
      config.password = ReadStringFromEEPROM(58);
      config.ntpServerName = ReadStringFromEEPROM(90);
      config.DeviceName = ReadStringFromEEPROM(122);
      config.OTApwd = ReadStringFromEEPROM(154);
      config.MQTTServer = ReadStringFromEEPROM(186);
      config.MQTTPort = EEPROMReadlong(218);
      config.HeartbeatTopic = ReadStringFromEEPROM(222);                // up to 32 Byte - EEPROM 260
      config.HeartbeatEvery = EEPROMReadlong(254);                  // 4 Byte - EEPROM 292
      config.rly1sensor = EEPROMReadint(258);                       // 2 Bytes - EEPROM 258
      config.rly1ontemp = EEPROMReadint(260);                       // 2 Bytes - EEPROM 260
      config.rly1offtemp = EEPROMReadint(262);                      // 2 Bytes - EEPROM 262
      config.rly2sensor = EEPROMReadint(264);                       // 2 Bytes - EEPROM 264
      config.rly2ontemp = EEPROMReadint(266);                       // 2 Bytes - EEPROM 266
      config.rly2offtemp = EEPROMReadint(268);                      // 2 Bytes - EEPROM 268
      // Application parameters here ... from EEPROM 192 to 511

      return true;

    }
    else
    {
      Serial.println("Configurarion NOT FOUND!!!!");
      return false;
    }
  }


#endif  // ESP8266

void printConfig(){

  Serial.println("Printing Config");

  Serial.printf("DHCP:%d\n", config.dhcp);
  Serial.printf("DayLight:%d\n", config.isDayLightSaving);

  Serial.printf("NTP update every %ld sec\n", config.Update_Time_Via_NTP_Every); // 4 Byte
  Serial.printf("Timezone %ld\n", config.timeZone); // 4 Byte

  Serial.printf("IP:%d.%d.%d.%d\n", config.IP[0],config.IP[1],config.IP[2],config.IP[3]);
  Serial.printf("Mask:%d.%d.%d.%d\n", config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3]);
  Serial.printf("Gateway:%d.%d.%d.%d\n", config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3]);


  Serial.printf("SSID:%s\n", config.ssid.c_str());
  Serial.printf("PWD:%s\n", config.password.c_str());
  Serial.printf("ntp ServerName:%s\n", config.ntpServerName.c_str());
  Serial.printf("Device Name:%s\n", config.DeviceName.c_str());
  Serial.printf("OTA password:%s\n", config.OTApwd.c_str());
  Serial.printf("MQTT Server:%s\n", config.MQTTServer.c_str());
  Serial.printf("MQTT Port:%ld\n", config.MQTTPort);
    // Application Settings here... from EEPROM 192 up to 511 (0 - 511)

}

void configLoadDefaults(uint16_t ChipId){

  #ifdef ARDUINO_ESP32_DEV
    config.ssid = "ESP32-" + String(ChipId,HEX);       // SSID of access point
  #elif ARDUINO_ESP8266_ESP01 || ARDUINO_ESP8266_NODEMCU
    config.ssid = "ESP8266-" + String(ChipId,HEX);       // SSID of access point
  #endif
  config.password = "" ;   // password of access point
  config.dhcp = true;
  config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 100;
  config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
  config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 254;
  config.ntpServerName = "0.ch.pool.ntp.org"; //"0.ch.pool.ntp.org"; // to be adjusted to PT ntp.ist.utl.pt
  config.Update_Time_Via_NTP_Every =  5;
  config.timeZone = 1;
  config.isDayLightSaving = true;
  config.DeviceName = "Not Named";
  config.OTApwd = "";
  config.MQTTServer = "192.168.1.6";
  config.MQTTPort = 1883;
  config.HeartbeatTopic = "hb";                // up to 32 Byte - EEPROM 260
  config.HeartbeatEvery = 0;                  // 4 Byte - EEPROM 292
  return;

}


#endif
