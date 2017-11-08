# Team RAD - Lab 3

Advanced Embedded Systems Course

## About

This lab was split into four parts. The first part set two of the wizzimotes clock onto pins. The clocks frequency was then measured in 1 second intervals for a minute. This was done using the Reading.py. The results can be found in RadTeam_Lab3_Part1_Data.xlsx. The second part of the lab was to visually demonstrate this desynchronization of the two clocks. This was done by uploading code which blinked the LEDs in second intervals to two separate wizzimotes. The third part of the lab was to implement a virtual clock. This was done using a timer and a quazi-global variable. The final part of the lab was to implement a synchronization protocol, which would take part 2 and part 3 of the lab and keep the two clocks synchronized. 

### Prerequisites

You will need to install wizzimote-contiki:

```
git clone https://suman2135@bitbucket.org/suman2135/wizzimote-contiki.git
```

Move the libmsp430.so into /usr/lib

Once this is installed you will need multiple packages for completing and running Contiki. They can be installed using:

```
sudo apt-get install build-essential binutils-msp430 gcc-msp430 msp430-libc msp430mcu mspdebug gcc-arm-none-eabi gdb-arm-none-eabi ant libncurses5-dev
```

## Authors

* **Carl Hildebrandt** - *Initial work* - [hildebrandt-carl](https://github.com/hildebrandt-carl)
* **Rebecca Horzewski** - *Initial work* - [rhorzewski](https://github.com/rhorzewski)
* **Eric Tran** - *Initial work* - [SilverShadowx](https://github.com/SilverShadowx)


