#ifndef ETHERNET_IP_LIST_H
#define ETHERNET_IP_LIST_H

#include <Ethernet.h>
#define MAX_LIST_NUM 60
#define BEGIN_IP 0

struct ListIP {
  int lenght = 0;
  struct {
    int SSID = 0;
    long temp = 0;
    long lumi = 0;
    int fan  = 0;
    int led = 0;
    long time = 0;
    IPAddress IP;  
  } listIP[MAX_LIST_NUM];
} listSensor, listOutput, listClient;

inline bool InitList(void) {
  for (int index = 0; index < MAX_LIST_NUM; index++) {
    listSensor.listIP[index].IP = IPAddress(10, 28, 2, BEGIN_IP + index);
    listOutput.listIP[index].IP = IPAddress(10, 28, 2, BEGIN_IP + index);
    listClient.listIP[index].IP = IPAddress(10, 28, 2, BEGIN_IP + index);
  }
  return true;
}
#endif // ETHERNET_IP_LIST_H
