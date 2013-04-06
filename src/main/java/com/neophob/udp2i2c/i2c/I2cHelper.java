package com.neophob.udp2i2c.i2c;

import java.io.IOException;

import com.neophob.udp2i2c.rainbowduino.RainbowduinoHelper;
import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;

public abstract class I2cHelper {

	/**
	 * sends data to an i2c slave
	 * 
	 * @param bus
	 * @param id
	 * @param buffer
	 * @throws IOException
	 * @throws InterruptedException 
	 */
	public static synchronized void sendData(I2CBus bus, int id, byte[] buffer) throws IOException, InterruptedException {
		//the buffer contains 24 bit images - we however only need 12 bit
		if (buffer==null) {
			System.out.println("warning, buffer was null!");
			return;
		}
		
		byte[] rb12bit = RainbowduinoHelper.convert24bitTo12bit(buffer);
		I2CDevice arduino = bus.getDevice(id);
		arduino.write(rb12bit, 0, rb12bit.length);
		Thread.sleep(5);
	}
	
	

}
