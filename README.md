# NtpTime

A simple NTP client.  Keeps the Particle Time synced to NTP.  This should not be used for
high precision time keeping.  The Particle Time is only one second resolution and
hardware limitations prevent high accuracy setting.  So the system time shouldn't be used for anything
that needs more than accuracy to a large fraction of a second.  

There is an interface that will return the time to millisecond resolution.  Accuracy of that
interface is primarily dependent on internet latency.

NtpTime uses one of Particle's software timers.
