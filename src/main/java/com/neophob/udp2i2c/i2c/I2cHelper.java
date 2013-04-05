package com.neophob.udp2i2c.i2c;

import java.io.IOException;

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
	 */
	public static void sendData(I2CBus bus, int id, byte[] buffer) throws IOException {
		I2CDevice arduino = bus.getDevice(id);
		arduino.write(buffer, 0, buffer.length);
	}
}
