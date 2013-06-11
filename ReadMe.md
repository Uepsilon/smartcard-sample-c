# SmartCard-Sample in C

This short code shall show how to connect to a smartcard and how to execute simple commands

## Depenencies 

*  Ubuntu 13.04
  * libpcsclite1
  * libpcsclite-dev

## Troubleshooting

##### winscard.h not found
1. Try installing libpcsclite-dev
2. Try include /usr/include/PCSC 
  * to PATH 
  * dirs for Header-Files (gcc via -I)