# piflow

Simple program to interface a Raspberry Pi with Piface digital hat to a cheap turbine flowmeter. The software is used to meter flow to a garden watering system.
The program blocks until:
- a certain number of litres have been passed or
- a specified time is exceeded or
- a specified maximum flow has been exceeded for 5 minutes

Building
$ git clone https://github.com/piface/libpifacedigital.git
$ git clone https://github.com/piface/libmcp23s17.git
$ git clone https://github.com/darrylb123/piflow.git

$ cd piface/libpifacedigital
$ make
$ cd piface/libmcp23s17
$ make
$ cd  piflow

Edit the build script to set the directory where the repositories went

$ build

The resulting program is called flow.



