#ifndef MY_FUNCTION_H
#define MY_FUNCTION_H

#include <Dhcp.h>                 // Dhcp library from: bjoern@cs.stanford.edu 12/30/2008
#include <Dns.h>                  // Dns library from: bjoern@cs.stanford.edu 12/30/2008
#include <Ethernet.h>             // If you use Ethernet Shield 2, use instead <Ethernet2.h> 
#include <EthernetUdp.h>          // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <SPI.h>                  // needed for Arduino versions later than 0018

void TelnetServer(EthernetServer &telnetServer, boolean &alreadyConnected, char telnetBuffer[], long &index, float tempData, float lumiData, int fanPWM, int ledPWM);
void WebServer(EthernetServer &webServer, boolean &alreadyConnected, float tempData, float lumiData, int fanPWM, int ledPWM);

#endif // MY_FUNCTION_H
