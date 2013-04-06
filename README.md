#udp-to-i2c

UDP-to-I2c Java server using netty, running on a raspberry pi.

##Main use case:
Recieve udp data from the PixelController software and send out on the i2c bus where some rainbowduinos are sitting.


##Setup RPI
http://neophob.com/2013/04/i2c-communication-between-a-rpi-and-a-arduino/


##Example

Listen on port 65506, use i2c bus 1 (common for rpi rev002) and send data to i2c address nr.4:

	pi@raspberrypi ~/test $ /home/pi/test/run.sh -p 65506 -b 1 -t 4 
	UDP-to-I2c Server v0.1 by Michael Vogt / neophob.com
	Bridge a TPM2Net UDP packet to I2C
	Listening on port 65506, using i2c bus 1, i2c target address:
	    4 


## Install Daemon

	sudo update-rc.d udp2i2c defaults