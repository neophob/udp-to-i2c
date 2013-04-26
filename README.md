#UDP-to-I2C

UDP-to-I2c Java server using netty, running on a Raspberry PI.

(c) by Michael Vogt 2013 / http://neophob.com

##Main use case:

Recieve UDP data from the PixelController software and send out on the I2C bus where some rainbowduinos are sitting.

This application use the TPM2.net UDP protocol, see http://www.ledstyles.de/ftopic18969.html for more details.

You can easily adapt the output to match your device.

##Setup RPI

Make sure the I2C module is loaded. I wrote a little guide here:

[http://neophob.com/2013/04/i2c-communication-between-a-rpi-and-a-arduino/](http://)

##Setup Arduino

You need to increase the Arduino I2C buffer size and speed, make sure you read the blog post above. After that load this firmware on your Rainbowduino (ignore the stupid name!):

You need to have the [Timer 1 Library installed](https://code.google.com/p/arduino-timerone/downloads/list)

[The Rainbowduino v3 firmware](https://github.com/neophob/udp-to-i2c/tree/master/src/build/Arduino/rv3_tmp)

Hint: don't forget to use a unique I2C address for each rainbowduino!


##Examples

Listen on port 65506, use i2c bus 1 (common for rpi model b rev002) and send data to i2c address nr. 4:

	pi@raspberrypi ~/udp2i2c $ ./run.sh -p 65506 -b 1 -d 5 -t 4 
	UDP-to-I2c Server v0.1 by Michael Vogt / neophob.com
	Bridge a TPM2Net UDP packet to I2C
	Listening on port 65506, using i2c bus 1, i2c target address:
	    4 

Listen on port 65506, use i2c bus 1 (common for rpi model b rev002) and send data to i2c address nr. 4, 5 and 6:

	pi@raspberrypi ~/udp2i2c $ ./run.sh -p 65506 -b 1 -d 5 -t 4:5:6
	UDP-to-I2c Server v0.1 by Michael Vogt / neophob.com
	Bridge a TPM2Net UDP packet to I2C
	Listening on port 65506, using i2c bus 1, i2c target address:
	    4 5 6

Listen on port 65506, use i2c bus 0 (common for rpi model b rev001), delay 5ms after a i2c data packed is sent out and send data to i2c address nr. 4, 5, 6, 7, 8 and 9:

    pi@raspberrypi ~/udp2i2c $ ./run.sh -p 65506 -b 0 -d 5 -t 5:7:6:9:8:4

## Install Daemon on RPI (run on boot)

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
	args="-p 65506 -b 0 -d 5 -t 5:7:6:9:8:4"
	application_dir="/home/pi/udp2i2c/"
	logfile="${application_dir}udp2i2c.log"

Hint: for RPI r001 (256MB ram) you need to use the I2C bus 0 (-b 0) while on the RPI r002 (512MB ram) you need to use the I2C buf 1 (-b 1).
	
## Performance

Test with one I2C output and 130 frames/s running on a RPI rev002. 
UDP2I2C use less than 8% CPU and about 4% of memory. 

Java Version:

	java version "1.8.0-ea"
	Java(TM) SE Runtime Environment (build 1.8.0-ea-b36e)
	Java HotSpot(TM) Client VM (build 25.0-b04, mixed mode)

## Example PixelController Config

You can download PixelController on the [Google Code Site](https://code.google.com/p/pixelcontroller/)

[Example PixelController config file](https://github.com/neophob/udp-to-i2c/blob/master/src/build/PixelController/config.properties)

## Errors

If you see this error:

	Failed to send data to i2c address 5.Packets recieved: 28026, errors: 384
	java.io.IOException: Error writing to /dev/i2c-0 at address 0x5. Got -20001.
		at com.pi4j.io.i2c.impl.I2CDeviceImpl.write(I2CDeviceImpl.java:90)
		at com.neophob.udp2i2c.i2c.I2cHelper.sendData(I2cHelper.java:29)
		at com.neophob.udp2i2c.udp.UdpServerHandler.messageReceived(UdpServerHandler.java:59)
		at org.jboss.netty.channel.SimpleChannelUpstreamHandler.handleUpstream(SimpleChannelUpstreamHandler.java:70)
		at org.jboss.netty.channel.DefaultChannelPipeline.sendUpstream(DefaultChannelPipeline.java:564)
		at org.jboss.netty.channel.DefaultChannelPipeline.sendUpstream(DefaultChannelPipeline.java:559)
		at org.jboss.netty.channel.Channels.fireMessageReceived(Channels.java:268)
		at org.jboss.netty.channel.socket.nio.NioDatagramWorker.read(NioDatagramWorker.java:98)
		at org.jboss.netty.channel.socket.nio.AbstractNioWorker.process(AbstractNioWorker.java:107)
		at org.jboss.netty.channel.socket.nio.AbstractNioSelector.run(AbstractNioSelector.java:312)
		at org.jboss.netty.channel.socket.nio.AbstractNioWorker.run(AbstractNioWorker.java:88)
		at org.jboss.netty.channel.socket.nio.NioDatagramWorker.run(NioDatagramWorker.java:343)
		at org.jboss.netty.util.ThreadRenamingRunnable.run(ThreadRenamingRunnable.java:108)
		at org.jboss.netty.util.internal.DeadLockProofWorker$1.run(DeadLockProofWorker.java:42)
		at java.util.concurrent.ThreadPoolExecutor.runWorker(ThreadPoolExecutor.java:1110)
		at java.util.concurrent.ThreadPoolExecutor$Worker.run(ThreadPoolExecutor.java:603)
		at java.lang.Thread.run(Thread.java:722)

This means that a I2C packed could not sent. Reason: device unavailable? Or try increasing the delay (-d option).