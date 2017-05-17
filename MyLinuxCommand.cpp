/*------------------------------------------- Sensor LDR Class --------------------------------------------*/
/* This define LinuxCommand class function
 * 
 * Created 2017
 * by Nguyen Thanh Long
 * Modified 13 April 2017 - version 1
 * by Nguyen Thanh Long
 * 
 * Reference: 
   --> https://www.arduino.cc/en/Reference/StringObject
 * 
 * This code is released in public domains
 */
#include "MyLinuxCommand.h"
#include <Dhcp.h>                 // Dhcp library from: bjoern@cs.stanford.edu 12/30/2008
#include <Dns.h>                  // Dns library from: bjoern@cs.stanford.edu 12/30/2008
#include <Ethernet.h>             // If you use Ethernet Shield 2, use instead <Ethernet2.h> 
#include <EthernetUdp.h>          // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <SPI.h>                  // needed for Arduino versions later than 0018

bool LinuxCommand::CheckCommand(String str) {
  String tempStr = str; 
  tempStr.trim();

  if ( tempStr.equalsIgnoreCase("ifconfig -a") ) { // Show ifconfig respond
      Serial.println("IP address: ");
      Serial.println(Ethernet.localIP());
      char output[500];
      Serial.println("More from log.txt: ");
      system("ifconfig > log.txt");
      FILE *fp;
      fp = fopen("log.txt", "r");
      while (fgets(output, 500, fp) != NULL) {
        Serial.println(output);
      }
      fclose(fp);

      return true;
    } else if ( tempStr.equalsIgnoreCase("netstat") || tempStr.equalsIgnoreCase("netstat -a") ) {  // Show netstat respond
      char output[500];
      Serial.println("Show listening port: ");
      system("netstat -a > log.txt");
      delay(5000);
      FILE *fp;
      fp = fopen("log.txt", "r");
      while (fgets(output, 500, fp) != NULL) {
        Serial.println(output);
      }
      fclose(fp);

      return true;
    } else if ( tempStr.equalsIgnoreCase("ls") ) {  // Show all file
      char output[500];
      Serial.println("Show Files: ");
      system("ls > log.txt");
      delay(1000);
      FILE *fp;
      fp = fopen("log.txt", "r");
      while (fgets(output, 500, fp) != NULL) {
        Serial.println(output);
      }
      fclose(fp);

      return true;
    } else if ( tempStr.equalsIgnoreCase("reboot") ) {  // Reboot the device
      Serial.println("System is restarting...");
      system("reboot");
      Serial.println("Success in restart");
      for(;;);
      return true;
    }

    return false;
}

