#include "ntp-time.h"

//
static char defaultURL[] = "time.nist.gov";

NtpTime::NtpTime()
{
  init(defaultUpdateMinutes,defaultURL);
}

NtpTime::NtpTime(int _updateMinutes) {
  init(_updateMinutes,defaultURL);
}

NtpTime::NtpTime(int _updateMinutes,char* _NTPServer)
{
  init(_updateMinutes,_NTPServer);
}


NtpTime::~NtpTime()
{
  this->stop();
}

void NtpTime::init(int _updateMinutes,char* _NTPServer)
{
  UDPClient = new UDP();
  this->setUpdateTimeMinutes(_updateMinutes);
  int strLen = min(serverNameSize,strlen(_NTPServer)+1);
  memcpy(serverName, _NTPServer, strLen);
}


void NtpTime::start()
{
  timer = new Timer(10000,&NtpTime::run,*this,FALSE);
  this->toWaitState();
}

void NtpTime::stop()
{
  if(timer)
  {
    timer->dispose();
    delete timer;
  }
}
/*
 *  This should be called from the software timer.
 *  It updates the system time at the requested interval.
 *  This functions as a state machine.  Each function handles some
 *  state and changes to a new state.
 */

void NtpTime::run()
{
    wait();
    issueRequest();
    requestIssued();
    responseReceived();
    updateSystemTime();  //This has to be before response recieved so the timer has a chance to delay.
}

/*
 *  Handle the waiting state by doing nothing until the wait is
 *  over, then going to the IssueRequest state.
 */
void NtpTime::wait()
{
  if(currentState == stateWaiting) //If it's time to update or we haven't yet updated, start an update.
  {
    currentState = stateIssueRequest;
  }
}

/*
 * Take us to the wait state
 */
void NtpTime::toWaitState()
{
  currentState = stateWaiting;
  //If we have synced, update slowly.  If we haven't update quickly.
  if(synced)
  {
    timer->changePeriod(updateTimeMinutes * 60000);
  }
  else
  {
    timer->changePeriod(10000);
  }
}




/*
 * Handle the IssueRequest state by, guess what, issuing a UDP request to the NTP server
 */
void NtpTime::issueRequest()
{
  if(currentState == stateIssueRequest)
  {
    UDPClient->begin(localPort);
    memset(packet, 0, packetSize);  //Zero out the UDP packet
    packet[0] =0x1B;   // Protocol version 3, client request

    UDPClient->beginPacket(serverName, 123); //NTP port 123
    UDPClient->write(packet,packetSize);
    UDPClient->endPacket();
    requestIssuedMillis = millis();
    timer->changePeriod(5);  // check for the response frequently
    currentState = stateRequestIssued;
  }
}
/*
 *  Handle the request issued state by checking if we have recieved
 *  a response and going to the response received state, timing out if we haven't
 *  haven't in a reasonable time.
 */
void NtpTime::requestIssued()
{
  if(currentState == stateRequestIssued)
  {
    if(!receiveResponse())
    {
      if(millis() > (requestIssuedMillis + 500))  // If the request has timed out
        {
          UDPClient->stop();
          toWaitState();
        }
      }
    }
  }

/*
 * Handle the response received state by parsing the response and
 * going to the update pending state if all is well or wait state if not.
 */

void NtpTime::responseReceived()
{
  if(currentState == stateResponseReceived)
  {
    // If kiss-of-death, just go back to wait state
    if (packet[1]==0) {
      toWaitState();
    }
    else
    {
      NTPTime = packet[40] << 24 | packet[41] << 16 | packet[42] << 8 | packet[43];
      NTPFrac = packet[44] << 24 | packet[45] << 16 | packet[46] << 8 | packet[47];
      if(NTPTime == 0)  //If we get a bogus time, just to back to wait state
      {
        toWaitState();
      }
      else
      {
        toUpdatePendingState();
      }
    }
  }
}


/*
 * Get the NTP response
 */
bool NtpTime::receiveResponse()
{
  bool received = false;
  int bytesrecv = UDPClient->parsePacket();
  if (bytesrecv>0) {
    received = true;
    responseReceivedMillis = millis();
    UDPClient->read(packet,packetSize);
    currentState = stateResponseReceived;
    UDPClient->stop();
  }
  return received;
}

/*
 *  Take us to the update pending state.
 */
void NtpTime::toUpdatePendingState()
{
    NTPMillis = (unsigned long)(((double)NTPFrac)  / 0xffffffff * 1000); //The fractional part of the NTP time in millisecond
    NTPMillis += (responseReceivedMillis - requestIssuedMillis)/2; //Adjust forward by half the round trip time.
    if(NTPMillis >= 1000)  //adjust for overflow
    {
      NTPMillis -= 1000;
      NTPTime += 1;
    }
    currentState = stateUpdatePending;
  }
/*
 *  Handle the update pending state by updating the system time, then going to the waiting state.
 */
void NtpTime::updateSystemTime()
{
  if(currentState == stateUpdatePending)
  {
    synced = TRUE;
    Time.setTime(NTPTime - ntpTimeEpochStart + 1);
    updateSystemMillis = millis();
    toWaitState();
  }
}

void NtpTime::setUpdateTimeMinutes(unsigned long intervalMinutes) {
  updateTimeMinutes = max(5, intervalMinutes);
}

/*
 * Return the current time to the second.  It should be the same as
 * Particle's Time.now();
 */
unsigned long NtpTime::now()
{
  if(!synced)
  {
    return Time.now();  //We haven't had a successful time request, so do the best we can.
  }
  return NTPTime - ntpTimeEpochStart + ((millis() - updateSystemMillis + NTPMillis) / 1000);
}

/*
 * Return the current time to the millisecond.
 */
void NtpTime::nowMillis(epochMillis* _now)
{
  if(!synced)
  {
    _now->seconds = Time.now();  //We haven't had a successful time request, so do the best we can.
    _now->millis = 0;
  }
  else
  {
    int nowMillis = (millis() - updateSystemMillis + NTPMillis);
    _now->seconds =  NTPTime - ntpTimeEpochStart + (nowMillis / 1000);
    _now->millis = nowMillis % 1000;
  }
  return;
}
