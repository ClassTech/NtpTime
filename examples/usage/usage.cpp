#include "ntp-time.h"

NtpTime* ntpTime;

String hhmmss(unsigned long int now)  //format value as "hh:mm:ss"
{
   String hour = String(Time.hourFormat12(now));
   String minute = String::format("%02i",Time.minute(now));
   String second = String::format("%02i",Time.second(now));
   return hour + ":" + minute + ":" + second;
};

void setup() {
    ntpTime = new NtpTime(15);  // Do an ntp update every 15 minutes;
    ntpTime->start();
}

void loop() {
    static unsigned long waitMillis;
    struct epochMillis now;  //holds the unix epoch time to millisecond resolution
    if(millis() > waitMillis) {
        ntpTime->nowMillis(&now);  //get the current NTP time
        Particle.publish("NTP clock is: ", hhmmss(now.seconds) + "." + String::format("%03i",now.millis));
        Particle.publish("System clock is: ", hhmmss(Time.now()));
        waitMillis = millis() + (15*1000);  // wait 15 seconds
    }
}
