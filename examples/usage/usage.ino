// Example usage for NtpTime library by ClassTech.

#include "NtpTime.h"

// Initialize objects from the lib
NtpTime ntptime;

void setup() {
/*
 * Start the NTP time synchronization.  By default, it will re-sync
 * every 10 minutes.
 */
    ntptime.start();
}

void loop() {
/*
 * Nothing needs to be done here.  NtpTime uses one of the system timers to
 * update the system clock.
 * ntptime.stop() will end the updating.
 * ntptime.now() will return the epoch seconds as seen by NTP.  It will be
 * slightly more accurate than the system now().
 */
}
