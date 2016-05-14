#include "ntp-time.h"

NtpTime* ntpTime;

void setup() {
    ntpTime = new NtpTime(15);  // Do an ntp update every 15 minutes;
    ntpTime->start();
}

void loop() {
    static unsigned long waitMillis;

    if(millis() > waitMillis) {
        Particle.publish("Clock is:", Time.now() );
        waitMillis = millis() + (15*60000);  // wait 15 minutes
    }
}
