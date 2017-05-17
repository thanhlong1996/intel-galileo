/*------------------------------------------- UDP Analytic Server --------------------------------------------*/
/* Serial control, UDP Analytic Server, Web Server and Telnet TCP/IP Server
 * Control stream output of Sensor, Output Client and User Telnet Client
 * Server can also be a Sensor or a Output Analytic Client

 * The circuit:
   --> Intel Galileo Broad Gen 2 with RJ-45 cat 5 or cat 6 ethernet cable

 * Advance circuit: (Uncomment TAG to use these function)
   --> Intel Galileo Broad Gen 2 connect to LM35 sensor on Pin A4 and LDR sensor on Pin A3 (TAG: $1$)
   --> Intel Galileo Broad Gen 2 connect to FAN on Pin D5 and LED on Pin D6 (TAG: $2$)
    
 * Created 2017
 * by Nguyen Thanh Long
 * Modified 27 April 2017 - version 2
 * by Nguyen Thanh Long

 * File require: Ass_3_server_part_v2.ino, MySensor.h, MySensor_LDR.cpp, MySensor_LM35.cpp, MyLinuxCommand.h, MyLinuxCommand.cpp
 * Assembling: Arduino 1.6.9
 * Reference link:
   --> http://arduino.cc/en/Tutorial/UDPSendReceiveString
   --> https://github.com/codebendercc/arduino-library-files/tree/master/libraries/Ethernet

 * Acronyms table:
   --> LDR - Photoresistor
   --> UDP_TX_PACKET_MAX_SIZE - 24 bytes
   
 * Caution: Only use sensor at normal room temperature (25 *C) for best accuracy (+- 0.5 *C)
 * Test bug tag: *0* *1* *2* *3* *4* *5_A* *5_B*
 * This code is released in private domains
 */

#include "MyFunction.h"

#include <string.h>

#include "MyLinuxCommand.h"       // Include if this is a Galileo module
#include "MySensor.h"             // Include MySensor.h to import sensor calculation function (TAG: $1$)

#include "EthernetIPList.h"       // It only use to access to IP more easily
#include "EthernetUdpBugFix.h"    // For fix bug only ( Some general library got some bugs )

//#define UDP_TX_PACKET_MAX_SIZE ..fill_in_the_blank..       // Uncomment to overide UDP_TX_PACKET_MAX_SIZE max size

////////////////////////////////CONST//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Limite check constant (contain condition border to check)
#define U_LUMI 300
#define L_LUMI 50
#define M_TEMP 30

#define END_OF_PACKET "@"
// Sensor constant
const int LED_PIN = 13;  // LED 13 PIN
const uint8_t LUMI_PIN = 3; // PIN AT LDR
const uint8_t TEMP_PIN = 4; // PIN AT LM35

// Ethernet constant
byte mac[] = {                // mac address of Ethernet module on board
  0x98 , 0x4F , 0xEE , 0x05 , 0x5E , 0x3F
};
IPAddress ip(10, 28, 3, 109); // ip host (Server ip)
IPAddress ip_broadcast(10, 28, 3, 85); // ip destination ( Broadcast ip )
IPAddress ip_sensor(10, 28, 3, 11); // ip destination ( Sensor ip )
IPAddress ip_output(10, 28, 3, 12); // ip destination ( Output ip )

IPAddress gateway_ip(10, 28, 2, 1); // gateway ip of local network ( usually Router IP )
IPAddress subnet_MyNetwork(255, 255, 254, 0); // subnet mask
IPAddress dns_MyNetwork(8, 8, 4, 4);  // Can be 8.8.4.4 or the same as gateway ip

// UDP constant
unsigned int localPort = 8888; // local port communication
unsigned int desPort = 8888;  // destination port communication
char ReplyBuffer[] = "ack"; // a string to send back
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////VARIABLE///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Udp Server
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet
EthernetUDP Udp; // An Ethernet instance to send and receive packet

// Server Web and Telnet
EthernetServer webServer(80);  // Http port
EthernetServer telnetServer(23); // Telnet port
char telnetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet
long indexTelnetBuffer = 0;
boolean alreadyConnected = false; // whether or not the client was connected previously

// Sensor buffer and reader
MySensor_LDR sensorLDR(LUMI_PIN);
MySensor_LM35 sensorLM35(TEMP_PIN);
float tempData = 0;
float lumiData = 0;
long timeRec = 0;

// Output Device 
int ledPWM = 0;
int fanPWM = 0;

// Auto Check Timer
long checkClock = 5000L;
long prevClock = 0;
unsigned long currentClock = 0;

// Led 13 blink Timer
long prevMillis = 0;                // will store last time LED was updated
unsigned long currentMillis = 0;    // current time since program started
long interval = 1000L;               // interval at which to blink (milliseconds)
int ledState = LOW;                 // ledState used to set the LED
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////BOOL CONDITION/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool isBlink = true;
bool showHelp = false;
bool isAuto = false;
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////FUNCTION/////////////////////////////////////////// (SEARCH TAG TO FIND DEFINE OF FUNCTION)
///////////////////////////////////////////////////////////////////////////////
void BlinkLed();                    // blinking led function (TAG: #0#)
void AutoSend();                    // auto send sensor data function (TAG: #1#)

void SendDataAnalyser(String dataInput = "", IPAddress ip_des = ip_broadcast); // send data to server (TAG: #2#)
bool CheckUserCommand();          // checking Server respond command (TAG: #3#)

void ReceiveDataAnalyser();         // receive data from UDP and send back "ack" (TAG: #4#)
void CheckControlCommand();         // checking Serial command function (TAG: #5#)
void RespondCommand();              // respond to some command function (TAG: #6#)

bool CheckSensorData(IPAddress &tempIP); // check sensor data input (TAG: #7#)
bool ResponeOutput(int index); // respone to output device (TAG: #8#)
bool CheckOutputData(IPAddress &tempIP); // check output data input (TAG: #9#)
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////MAIN FUNCTION//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void setup() {
  // get IP list
  InitList();
  
  // set the digital pin as output: (For test function purpose)
  pinMode(LED_PIN, OUTPUT);

  // initialize serial at 9600 bits per second, say HELLO
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial port to connect. Only need for Leonardo
  }
  Serial.setTimeout(10000); // Serial Input Timeout in 10 seconds
  Serial.println("Welcome to Galileo Gen 2 broad");

  // initialize ethernet 
  Serial.println("Initalizing Ethernet connection...");
  /////////////////////////////////////DHCP REQUEST BLOCK/////////////////////////////////////////
//  if (Ethernet.begin(mac) == 0) { 
//    Serial.println("Fail to request DHCP. Using Static request");
//    Ethernet.begin(mac, ip, dns_MyNetwork, gateway_ip, subnet_MyNetwork); // *5_A* Input gateway is also DNS server
//  } else {
//    Serial.println("Success on DHCP request");
//  }
  ////////////////////////////////////////////////////////////////////////////////////////////////
  Ethernet.begin(mac, ip, dns_MyNetwork, gateway_ip, subnet_MyNetwork); // *5_B* Input gateway is also DNS server 
//  Ethernet.begin(mac, ip);  // *5_C* Input MAC and IP only to know Gateway
  Serial.println("Initalizing Ethernet done!");
  Serial.print("Connect with local address: ");
  Serial.println(Ethernet.localIP());

  // initialize Web and Telnet Server
  Serial.println("Initalizing Web and Telnet Server...");
  webServer.begin();
  telnetServer.begin();
  Serial.println("Initalizing Server done!");
  
  // initialize Udp protocol 
  Serial.println("Initalizing Udp protocol...");
  if (Udp.begin(localPort) == 0) {
    Serial.println("Fail to make protocol! Reset to retry.");
    for (;;); // stop here when there is an error in initialize step
  }
  Serial.println("Initalizing Udp protocol done!");
  delay(1000);

  Serial.print("Sensor Client/Server Center ready with address: ");
  Serial.println(Ethernet.localIP());
  
}

void loop() {
  RespondCommand();
  ReceiveDataAnalyser();
  CheckControlCommand();

  TelnetServer(telnetServer, alreadyConnected, telnetBuffer, indexTelnetBuffer, tempData, lumiData, fanPWM, ledPWM);
  WebServer(webServer, alreadyConnected, tempData, lumiData, fanPWM, ledPWM);
}
///////////////////////////////////////////////////////////////////////////////

void AutoSend() { // Only automatically send sensor data (TAG: #1#)
  currentClock = millis(); // update time since program started ( need to be closest to the last section using it )
  if (currentClock - prevClock > checkClock) {
    // save the last time you send data
    prevClock = currentClock;

    ///////////////////TEST BEFORE SEND BLOCK/////////////////////////// (Uncomment block to test)
//    Serial.println("Try reading sensor data:");
//    Serial.print("LDR test = "); Serial.println(lumiData = sensorLDR.readLux());
//    Serial.print("LM35 test = "); Serial.println(tempData = sensorLM35.readTemperature());
    ////////////////////////////////////////////////////////////////////

    String data = "_LUX:" + String((int)lumiData, DEC) + END_OF_PACKET + "_TEMP:" + String((int)tempData, DEC) + END_OF_PACKET;
    SendDataAnalyser(data);
  }
}

void BlinkLed() { // Only for checking if Galileo is function correctly or not (TAG: #0#)
  currentMillis = millis(); // update time since program started ( need to be closest to the last section using it )
  if (currentMillis - prevMillis > interval) {
    // save the last time you blinked the LED
    prevMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    ledState = !ledState;
    // set the LED with the ledState of the variable:
    digitalWrite(LED_PIN, ledState);
  }
}

void ReceiveDataAnalyser() { // (TAG: #4#)
  // *3* Read receive data
  int packetSize = Udp.parsePacket();
  if (Udp.available()) {    // can be replace by checking ( packetSize == 0 ) ?
    IPAddress tempIP = ip_broadcast;
    if ( Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE)  == -1) {
      Serial.println("Not success in receiving data. Weak or error in connection.");
    } else {
      Serial.println("Contents:");
      Serial.println(packetBuffer);
      if ( CheckUserCommand() ) { // Check User command and respond
        // Do nothing
      } else if ( CheckSensorData(tempIP) ){
        // Do nothing
      } else if ( CheckOutputData(tempIP) ){
        // Do nothing
      } else {
        Serial.println("Unknown Command or Data...");
      }
    }
    
    Serial.print("Receive packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = tempIP; // Some bug error with Udp.remotePort() and Udp.remoteIP() so use alternated ways
    for (int i = 0; i < 4; i++) {
      Serial.print(remote[i], DEC); // if use Udp.remoteIP() -> 255.255.255.255 sometimes is seen
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(desPort);

    memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
  }
  // *3* End receive data
}

void CheckControlCommand() { // (TAG: #5#)
  if (Serial.available() > 0) {
    String str = Serial.readStringUntil('\n');
    str.trim(); 
    Serial.println(str);

    if ( str.equalsIgnoreCase("ifconfig") ) { // Show ifconfig respond
      Serial.println("IP address: ");
      Serial.println(Ethernet.localIP());
    } else if ( LinuxCommand::CheckCommand(str) == true) { // Comment this to use for Arduino
      // *4* Linux ifconfig information ( only available for Galileo broad - comment to use for other Arduino broad )
      // Do nothing
    } else if ( str.equalsIgnoreCase("blink") ) { // Blink Led
      isBlink = !isBlink;
    } else if ( str.equalsIgnoreCase("help") ) { // Show help
      showHelp = true;
    } else if ( str.substring(0, 5) == "send " ) {  // Chat with Some User by UDP
      String data = str.substring(5);
      if (data == "lumi") {
        data = "_LUX:" + String((int)lumiData, DEC) + END_OF_PACKET;
      } else if (data == "temp") {
        data = "_TEMP:" + String((int)tempData, DEC) + END_OF_PACKET;
      } else if (data == "fan") {
        data = "_FAN:" + String((int)fanPWM, DEC) + END_OF_PACKET;
      } else if (data == "led") {
        data = "_LED:" + String((int)ledPWM, DEC) + END_OF_PACKET;
      }
      SendDataAnalyser(data);
    } else if ( str.equalsIgnoreCase("all") ) { // Send all data to all Client
      // TODO: Something
    } else if ( str.equalsIgnoreCase("auto on") ) { // Auto check sensor data
      isAuto = true;
    } else if ( str.equalsIgnoreCase("auto off") ) { // Auto check sensor data
      isAuto = false;
    } else {
      Serial.println("Content Invalid or Invalid Command");
    }

  }
}

void RespondCommand() { // (TAG: #6#)
  if (isBlink) {
    BlinkLed();
  } else if (isAuto) {
    AutoSend();
  } else if (showHelp) {
    Serial.println("Help:");
    Serial.println("-Enter-> help    \t: To show help");
    Serial.println("-Enter-> send    \t: To show chat");
    Serial.println("-Enter-> blink   \t: To blink");
    Serial.println("-Enter-> ifconfig\t: To show system device config");
    Serial.println("-Enter-> netstat \t: To show listenning port");
    Serial.println("-Enter-> auto    \t: To send sensor data");
    showHelp = false;
  }
}

bool CheckUserCommand() { // (TAG: #3#)
  if (strncmp(packetBuffer, "auto", 4) == 0) {
    isAuto = !isAuto;
  } else if (strncmp(packetBuffer, "temp", 4) == 0) {
    String data = "_TEMP:" + String((int)tempData, DEC) + END_OF_PACKET;
    SendDataAnalyser(data);
  } else if (strncmp(packetBuffer, "lumi", 4) == 0) {
    String data = "_LUX:" + String((int)lumiData, DEC) + END_OF_PACKET;
    SendDataAnalyser(data);
  } else if (strncmp(packetBuffer, "fan", 3) == 0) {
    String data = "_FAN:" + String((int)fanPWM, DEC) + END_OF_PACKET;
    SendDataAnalyser(data);
  } else if (strncmp(packetBuffer, "led", 3) == 0) {
    String data = "_LED:" + String((int)ledPWM, DEC) + END_OF_PACKET;
    SendDataAnalyser(data);
  } else {
    return false;
  }
  return true;
}

bool CheckSensorData(IPAddress &tempIP) {  // (TAG: #7#)
  String tempStr = String(packetBuffer); tempStr.trim();
  int tempIndex = 0;
  int sensorNum = -1;
  int index;
  if ( (index = tempStr.indexOf("sensor")) != -1) {
    if (tempStr.substring(index+6,tempStr.indexOf('_')) != "") {
      sensorNum = tempStr.substring(index+6,tempStr.indexOf('_')).toInt();
      Serial.println("Got data from sensor number " + String(sensorNum));
      tempIP = listSensor.listIP[sensorNum].IP;
      if (listSensor.lenght < sensorNum) listSensor.lenght = sensorNum;
    }
    if ( (tempIndex = tempStr.indexOf("_TIME:")) != -1) {
      timeRec = tempStr.substring(tempIndex+6,tempStr.indexOf(END_OF_PACKET,tempIndex)).toInt();
      if (sensorNum != -1) listSensor.listIP[sensorNum].time = timeRec;
    }
    if ( (tempIndex = tempStr.indexOf("_IP:")) != -1) {
      tempIP = ip_sensor = IPAddress(10,28,2,tempStr.substring(tempIndex+4,tempStr.indexOf(END_OF_PACKET,tempIndex)).toInt()); // Last octect number only
      if (sensorNum != -1) listSensor.listIP[sensorNum].IP = ip_sensor;
    }
    if ( (tempIndex = tempStr.indexOf("_TEMP:")) != -1) {
      tempData = tempStr.substring(tempIndex+6,tempStr.indexOf(END_OF_PACKET,tempIndex)).toInt();
      if (sensorNum != -1) listSensor.listIP[sensorNum].temp = tempData;
      ResponeOutput(sensorNum);
    }
    if ( (tempIndex = tempStr.indexOf("_LUMI:")) != -1) {
      lumiData = tempStr.substring(tempIndex+6,tempStr.indexOf(END_OF_PACKET,tempIndex)).toInt();
      if (sensorNum != -1) listSensor.listIP[sensorNum].lumi = lumiData;
      ResponeOutput(sensorNum);
    }
  } else {
    return false;
  }
  return true;
}

void SendDataAnalyser(String dataInput, IPAddress ip_des) { // (TAG: #2#)
  String data = dataInput;
  char inputPacket[UDP_TX_PACKET_MAX_SIZE];
  data.toCharArray(inputPacket, UDP_TX_PACKET_MAX_SIZE);
  Serial.println("Sent:");
  Serial.println(data);

  // *1* Send data to gateway
  Udp.beginPacket(ip_des, desPort);
  size_t sendByte = Udp.write(inputPacket);   // size_t <=> unsigned int
  Udp.endPacket();
  // *1* Finish data send

  // *2* Show ip gateway to Serial
  Serial.print("To: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(ip_des[i], DEC);
    if (i < 3) {
      Serial.print(".");
    }
  }
  Serial.print(", send byte: ");
  Serial.println(sendByte);
}

bool ResponeOutput(int index) {  // (TAG: #8#)
  // Check Led respond
  if ( lumiData < L_LUMI ) {
    ledPWM = 255; // 100%
  } else if ( lumiData < U_LUMI ) {
    ledPWM = 127; // 50%
  } else {
    ledPWM = 0; // 0%
  }
  // Check fan respond
  if ( tempData > M_TEMP ) {
    fanPWM = 1;
  } else {
    fanPWM = 0;
  }
  
  String temp = (index == -1) ?  "" : (String(index)); 
  String data = "output" + temp + "_FAN:" + String(fanPWM) + END_OF_PACKET + "_LED:" + String(ledPWM) + END_OF_PACKET;
  IPAddress tempIP = ip_output;
  if (index != -1) {
    tempIP = listOutput.listIP[index].IP;
    listOutput.listIP[index].fan = fanPWM;
    listOutput.listIP[index].led = ledPWM;
  }
  SendDataAnalyser(data, tempIP);
  
  return true;
}

bool CheckOutputData(IPAddress &tempIP) { // (TAG: #9#)
  String tempStr = String(packetBuffer); tempStr.trim();
  int tempIndex = 0;
  int outputNum = -1;
  int index;
  if ( (index = tempStr.indexOf("output")) != -1) {
    if (tempStr.substring(index+6,tempStr.indexOf('_')) != "") {
      outputNum = tempStr.substring(index+6,tempStr.indexOf('_')).toInt();
      Serial.println("Got data from sensor number " + String(outputNum));
      tempIP = listOutput.listIP[outputNum].IP;
      if (listOutput.lenght < outputNum) listOutput.lenght = outputNum;
    }
    if ( (tempIndex = tempStr.indexOf("_TIME:")) != -1) {
      timeRec = tempStr.substring(tempIndex+6,tempStr.indexOf(END_OF_PACKET,tempIndex)).toInt();
      if (outputNum != -1) listOutput.listIP[outputNum].time = timeRec;
    }
    if ( (tempIndex = tempStr.indexOf("_IP:")) != -1) {
      tempIP = ip_output = IPAddress(10,28,2,tempStr.substring(tempIndex+4,tempStr.indexOf(END_OF_PACKET,tempIndex)).toInt()); // Last octect number only
      if (outputNum != -1) listOutput.listIP[outputNum].IP = ip_sensor;
    }
    if ( (tempIndex = tempStr.indexOf("_FAN:")) != -1) {
      fanPWM = tempStr.substring(tempIndex+5,tempStr.indexOf(END_OF_PACKET,tempIndex)).toInt();
      if (outputNum != -1) listOutput.listIP[outputNum].temp = tempData;
    }
    if ( (tempIndex = tempStr.indexOf("_LED:")) != -1) {
      ledPWM = tempStr.substring(tempIndex+5,tempStr.indexOf(END_OF_PACKET,tempIndex)).toInt();
      if (outputNum != -1) listOutput.listIP[outputNum].lumi = lumiData;
    }
    SendDataAnalyser(String(ReplyBuffer), tempIP);
  } else {
    return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////// END
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
