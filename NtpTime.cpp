/* NtpTime by Douglas Johnson

   Keeps the Particle Time synced to NTP.  This should not be used for
   high precision time keeping.  The Particle Time is only one second resolution.
   While NtpTime tries to set that close to the tick of the second, there
   are too many variables to have any real confidence that that happens.  So
   this shouldn't be used for anything that needs more than acccuracy to the
   second.  NtpTime uses one of Particle's software timers.

   Example:
   #include "NtpTime.h"

   NtpTime* ntpTime;
   void setup()
   {
     ntpTime = new NtpTime(15);  Do an ntp update every 15 minutes;
     ntpTime->start();
   }

   void loop()
   {
     static unsigned long waitMillis;
     if(millis() > waitMillis)
     {
       Particle.publish("Clock is:", Time.now() );
       waitMillis = millis() + (15*60000);  //wait 15 minutes
     }
   }

   Copyright (c) 2016 Douglas Johnson.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
     AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
     ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
     POSSIBILITY OF SUCH DAMAGE.
*/

#include "NtpTime.h"

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
    updateSystemTime();  //This has to be before response recieved so the timer has a chance to delay.
    responseReceived();
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
 *  Here we figure the millis that we need to update the system time
 *  in order to do it as close to the zero tick as possible.
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
    timer->changePeriod(1000 - NTPMillis);  //When to update the system.
    NTPTime += 1;                      //The "+1" allows for our delay to try and set the time on the tick of the second.
    currentState = stateUpdatePending;
  }
/*
 *  Handle the update pending state by doing nothing until the right millis,
 *  updating the system time, then going to the waiting state.
 */
void NtpTime::updateSystemTime()
{
  if(currentState == stateUpdatePending)
  {
    synced = TRUE;
    Time.setTime(NTPTime - ntpTimeEpochStart);
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
