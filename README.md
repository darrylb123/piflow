# piflow

Simple program to interface a Raspberry Pi with Piface digital hat to a cheap turbine flowmeter. The software is used to meter water flow to a garden watering system. This is very useful in the event of pipe failure.
The program blocks until:
- a certain number of litres have been passed or
- a specified time is exceeded or
- a specified maximum flow has been exceeded for 5 minutes

```
Building
$ git clone https://github.com/darrylb123/piflow.git
$ cd piflow
$ git clone https://github.com/piface/libpifacedigital.git
$ git clone https://github.com/piface/libmcp23s17.git
$ make
```

The resulting program is called piflow.
flow requires the following arguments:
- -t Timeout in seconds (The program will exit when this timeout is exceeded)
- -p Pulses per litre ( Calibration constant for the particular flowmeter )
- -l number of litres measured before exit
- -m Maximum flow rate in litres/min. ( program exits if this flow rate is exceeded for 5 minutes. Resets the 5 minutes if flow drops below maximum. )
## Implementation.
The flow meter is connected to input 1 via a resistive voltage divider to limit the input voltage to 3.3V. My flowmeter requires 5DDC. Use one of the many voltage divider calculators on the Internet to choose your resistors. Make sure they are in kilohms to avoid damaging the flowmeter. Test the voltage before hooking up to the piface as damage to the piface will occur if greater than 3.3v.

Calibration of flowmeter in PPL (Pulses per litre)
Run flow with the flow meter connected with estimated arguments. Fill a known capacity container, say 20 litre bucket and turn off the water as the container fills to the known volume (ie brim).
The program will exit when the timeout occurs and display the number of pulses counted. Divide the number of pulses counted by the number of litres and you have the pulses per litre. Round the number to the nearest whole number and you have it.

eg 
```
$ piflow -t 1000 -p 200 -l 200 -m 1000 #  1000 seconds or 200 litres  max 1000 l/m
Turn on the water
Turn off the water when full


Display will look like:
2.03       250 11094 44 26.7 0
3.05       250 17808 71 26.9 0
Fri 23 Mar 16:07:39 AEST 2018, 1.10, 8751, 35.00, 31.82
```

You are interested in  the 3rd column in the CSV output, in this case 8751 pulses. 

