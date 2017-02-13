# NtpTime

Keeps the Particle Time synced to NTP.  This should not be used for
high precision time keeping.  The Particle Time is only one second resolution. So
this shouldn't be used for anything that needs more than acccuracy to a large
fraction of a second.  NtpTime uses one of Particle's software timers.

## Usage

Connect XYZ hardware, add the NtpTime library to your project and follow this simple example:

```
// Example usage for NtpTime library by ClassTech.

#include "NtpTime.h"

// Initialize objects from the lib
Ntptime ntptime;

void setup() {
    // Call functions on initialized library objects that require hardware
    ntptime.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    ntptime.process();
}
```


## Documentation

Keeps the Particle Time synced to NTP. This should not be used for high precision time keeping. The Particle Time is only one second resolution. So this shouldn't be used for anything that needs more than acccuracy to a large fraction of a second. NtpTime uses one of Particle's software timers.

## Contributing

Here's how you can make changes to this library and eventually contribute those changes back.

To get started, [clone the library from GitHub to your local machine](https://help.github.com/articles/cloning-a-repository/).

Change the name of the library in `library.properties` to something different. You can add your name at then end.

Modify the sources in <src> and <examples> with the new behavior.

To compile an example, use `particle compile examples/usage` command in [Particle CLI](https://docs.particle.io/guide/tools-and-features/cli#update-your-device-remotely) or use our [Desktop IDE](https://docs.particle.io/guide/tools-and-features/dev/#compiling-code).

After your changes are done you can upload them with `particle library upload` or `Upload` command in the IDE. This will create a private (only visible by you) library that you can use in other projects. Do `particle library add ntp-time_myname` to add the library to a project on your machine or add the ntp-time_myname library to a project on the Web IDE or Desktop IDE.

At this point, you can create a [GitHub pull request](https://help.github.com/articles/about-pull-requests/) with your changes to the original library. 

If you wish to make your library public, use `particle library publish` or `Publish` command.

## LICENSE
Copyright 2017 ClassTech

Licensed under the MIT license
