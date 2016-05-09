/* NtpTime by Douglas Johnson

   Keeps the Particle Time synced to NTP.  This should not be used for
   high precision time keeping.  The Particle Time is only one second resolution.
   While NtpTime tries to set that close to the tick of the second, there
   are too many variables to have any real confidence that that happens.  So
   this shouldn't be used for anything that needs more than acccuracy to a large
   fraction of a second.  NtpTime uses one of Particle's software timers.

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
