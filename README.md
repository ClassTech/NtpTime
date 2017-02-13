# NtpTime

Keeps the Particle Time synced to NTP.  This should not be used for
high precision time keeping.  The Particle Time is only one second resolution. So
this shouldn't be used for anything that needs more than acccuracy to a large
fraction of a second.  NtpTime uses one of Particle's software timers.

## Usage

Add the NtpTime library to your project and follow this simple example:

```
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
```


## LICENSE
Copyright 2017 ClassTech

Licensed under the MIT license
