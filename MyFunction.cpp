#include "MyFunction.h"

#define END_OF_PACKET "@"

bool CheckTelnetRespond(EthernetServer &telnetServer, char telnetBuffer[], long index, float tempData, float lumiData, int fanPWM, int ledPWM) {
  String data;
  if (strncmp(telnetBuffer, "temp", 4) == 0) {
    data = "_TEMP:" + String((int)tempData, DEC) + END_OF_PACKET;
  } else if (strncmp(telnetBuffer, "lumi", 4) == 0) {
    data = "_LUX:" + String((int)lumiData, DEC) + END_OF_PACKET;
  } else if (strncmp(telnetBuffer, "fan", 3) == 0) {
    data = "_FAN:" + String((int)fanPWM, DEC) + END_OF_PACKET;
  } else if (strncmp(telnetBuffer, "led", 3) == 0) {
    data = "_LED:" + String((int)ledPWM, DEC) + END_OF_PACKET;
  } else {
    return false;
  }
  char tempCharArray[255];
  data.toCharArray(tempCharArray, data.length());
  for (int i = 0; i < data.length(); i++) telnetServer.write(tempCharArray[i]);
  return true;
}

void TelnetServer(EthernetServer &telnetServer, boolean &alreadyConnected, char telnetBuffer[], long &index, float tempData, float lumiData, int fanPWM, int ledPWM) {
  // wait for a new client:
  EthernetClient client = telnetServer.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clead out the input buffer:
      client.flush();    
      Serial.println("We have a new client");
      client.println("Hello, client!"); 
      alreadyConnected = true;
    } 

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = client.read();
      // echo the bytes back to the client:
      telnetServer.write(thisChar);
      // echo the bytes to the server as well:
      Serial.write(thisChar);
      if (thisChar == '\n') {
        String tempResp = String("Server Receive: ");
        char tempCharArray[255];
        tempResp.toCharArray(tempCharArray, tempResp.length());
        for (int i = 0; i < tempResp.length(); i++) telnetServer.write(tempCharArray[i]);
        for (int i = 0; i < index; i++) telnetServer.write(telnetBuffer[i]);
        CheckTelnetRespond(telnetServer, telnetBuffer, index, tempData, lumiData, fanPWM, ledPWM);
        index = 0;
        memset(telnetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Got end of buffer signal");
      } else {
        telnetBuffer[index++] = thisChar;
      }
    }
  }
}

void WebServer(EthernetServer &webServer, boolean &alreadyConnected, float tempData, float lumiData, int fanPWM, int ledPWM) {
  // listen for incoming clients
  EthernetClient client = webServer.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          // output System time
            client.print("<head><title>Welcome to Galileo Server</title></head>\n");
            client.print("<body><h1>Welcome to Galileo Server</h1>");
            client.print("<h2>System time now is:");
            client.print(millis());
            client.print(" ms </h2>");
            //client.println("<br />");  
          // output the value of each analog input pin
            client.print("<p>Temp ");
            client.print(" is ");
            client.print(tempData);
            client.print(" *C");
            client.println("<br />");      
            client.print("Lumi ");
            client.print(" is ");
            client.print(lumiData);
            client.print(" lux");
            client.println("<br /></p>");
          // show state of output
            client.print("<p>FAN ");
            client.print(" is ");
            client.print( (fanPWM == 1) ? "on" : "off");
            client.println("<br />");      
            client.print("LED ");
            client.print(" is at ");
            client.print( (ledPWM == 255) ? "100%" : ((ledPWM == 127) ? "50%" : "0%"));
            client.println("<br /></p></body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

