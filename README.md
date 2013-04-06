#UDP-to-I2C

UDP-to-I2c Java server using netty, running on a raspberry pi.

(c) by Michael Vogt 2013 / http://neophob.com

##Main use case:
Recieve udp data from the PixelController software and send out on the i2c bus where some rainbowduinos are sitting.

This application use the TPM2.net UDP protocol, see http://www.ledstyles.de/ftopic18969.html for more details.

##Setup RPI
http://neophob.com/2013/04/i2c-communication-between-a-rpi-and-a-arduino/


##Example

Listen on port 65506, use i2c bus 1 (common for rpi rev002) and send data to i2c address nr. 4:

	pi@raspberrypi ~/test $ /home/pi/test/run.sh -p 65506 -b 1 -t 4 
	UDP-to-I2c Server v0.1 by Michael Vogt / neophob.com
	Bridge a TPM2Net UDP packet to I2C
	Listening on port 65506, using i2c bus 1, i2c target address:
	    4 

Listen on port 65506, use i2c bus 1 (common for rpi rev002) and send data to i2c address nr. 4, 5 and 6:

	pi@raspberrypi ~/test $ /home/pi/test/run.sh -p 65506 -b 1 -t 4:5:6
	UDP-to-I2c Server v0.1 by Michael Vogt / neophob.com
	Bridge a TPM2Net UDP packet to I2C
	Listening on port 65506, using i2c bus 1, i2c target address:
	    4 5 6

## Install Daemon (run on boot)
Copy the `init.d/udp2i2c` file to `/etc/init.d/` (as root), then:

	pi@raspberrypi ~/test $ sudo update-rc.d udp2i2c defaults
	update-rc.d: using dependency based boot sequencing
	insserv: warning: script 'K01udp2i2c' missing LSB tags and overrides
	insserv: warning: script 'udp2i2c' missing LSB tags and overrides

	
Check in which runlevels the daemon is started:

	pi@raspberrypi ~/test $ find /etc/rc?.d | grep udp2i2c
	/etc/rc0.d/K01udp2i2c
	/etc/rc1.d/K01udp2i2c
	/etc/rc2.d/S02udp2i2c
	/etc/rc3.d/S02udp2i2c
	/etc/rc4.d/S02udp2i2c
	/etc/rc5.d/S02udp2i2c
	/etc/rc6.d/K01udp2i2c
	
Open the init script `/etc/init.d/udp2i2c` and make sure the following entries are correct:

	JAVA_HOME="/usr/local/java/"
	args="-p 65506 -b 1 -t 4"
	application_dir="/home/pi/test/"
	
	