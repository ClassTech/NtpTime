#pragma once

#include "application.h"

#define SERVER_NAME_SIZE 64
#define PACKET_SIZE 48

/*
 * The unix epoch time to millisecond resolution
 */
struct epochMillis{
  unsigned long int seconds;
  unsigned int millis;
};

class NtpTime {
public:
  NtpTime();
  NtpTime(int _updateMinutes);
  NtpTime(int _updateMinutes, char* _NTPServer);
  ~NtpTime();
  void setUpdateTimeMinutes(unsigned long _intervalMinutes);;
  void start();
  void stop();
  unsigned long now();
  void nowMillis(epochMillis* _now);

private:
  UDP* UDPClient;
  Timer* timer;
  const int serverNameSize = SERVER_NAME_SIZE;
  char serverName[SERVER_NAME_SIZE]; // Name of ntpServer
  char currentState;                 // State of the state machine
  const char stateWaiting = 0;
  const char stateIssueRequest = 1;
  const char stateRequestIssued = 2;
  const char stateResponseReceived = 3;
  const char stateUpdatePending = 4;
  bool synced = FALSE;
  const int minUpdateMinutes = 5;   //The minimum update interval in minutes.  NTP servers don't like to bothered very often.
  const unsigned long ntpTimeEpochStart = 2208988800UL;
	unsigned long responseReceivedMillis;
  unsigned long requestIssuedMillis;
  unsigned long lastSyncMillisTime;
  unsigned long NTPTime;
  unsigned long NTPFrac;
  unsigned long NTPSeconds;
  unsigned long NTPMillis;
  unsigned long updateSystemMillis;
  unsigned long updateTimeMinutes;
  unsigned long localPort =  2390;  //Local port for the UDP request
  const int packetSize = PACKET_SIZE;
  unsigned char packet[PACKET_SIZE]; //UDP packet
  void wait();
  void run();
  void updateSystemTime();
	void issueRequest();
  void requestIssued();
  void responseReceived();
	bool receiveResponse();
  void toUpdatePendingState();
  void toWaitState();
  const int defaultUpdateMinutes = 10;
	void init(int _updateMinutes,char* _NTPServer);
};
