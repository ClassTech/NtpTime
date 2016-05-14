# NtpTime

Keeps the Particle Time synced to NTP.  This should not be used for
high precision time keeping.  The Particle Time is only one second resolution.
While NtpTime tries to set that close to the tick of the second, there
are too many variables to have any real confidence that that happens.  So
this shouldn't be used for anything that needs more than acccuracy to a large
fraction of a second.  NtpTime uses one of Particle's software timers.
