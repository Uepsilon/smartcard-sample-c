# SmartCard-Sample in C

This short code shall show how to connect to a smartcard and how to execute simple commands

## Depenencies

*  Ubuntu 13.04
  * libpcsclite1
  * libpcsclite-dev


## Troubleshooting / FAQ

##### What is Winscard?
It is Part of PCSC-Lite and implements an Interface to Cardreaders and Cards. Further
Details can be found [here](http://pcsclite.alioth.debian.org/api/index.html)

##### winscard.h not found
1. Try installing libpcsclite-dev
2. Try include /usr/include/PCSC
  * to PATH
  * dirs for Header-Files (gcc via -I)
