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
	 * @throws InterruptedException 
	 */
	public static synchronized void sendData(I2CBus bus, int id, byte[] buffer) throws IOException, InterruptedException {
		//the buffer contains 24 bit images - we however only need 12 bit
		if (buffer==null) {
			System.out.println("warning, buffer was null!");
			return;
		}
		
		byte[] rb12bit = convert24bitTo12bit(buffer);
		I2CDevice arduino = bus.getDevice(id);
		arduino.write(rb12bit, 0, rb12bit.length);
		//Thread.sleep(5);
	}
	
	
	/**
	 * the rainbowduino can control 12bit per pixel - reduce the data rate here
	 * output format is [32*r][32*g][32*b]
	 * 
	 * @param data
	 * @return
	 */
	public static byte[] convert24bitTo12bit(byte[] data) {
		if (data.length != 192) {
			System.out.println("warning, unexpected buffersize: "+data.length);
			return new byte[0];
		}
		
		int[] r = new int[64];
		int[] g = new int[64];
		int[] b = new int[64];

		int i=0;
		int j=0;
		while (i<192) {
			r[j] = data[i++];
			g[j] = data[i++];
			b[j] = data[i++];
			j++;
		}
		
		byte[] converted = new byte[96];
		int ofs=0;
		int dst=0;		
		for (i=0; i<32;i++) {
			//240 = 11110000 - delete the lower 4 bits, then add the (shr-ed) 2nd color
			converted[00+dst] = (byte)(((r[ofs]&240) + (r[ofs+1]>>4))& 255); //r
			converted[32+dst] = (byte)(((g[ofs]&240) + (g[ofs+1]>>4))& 255); //g
			converted[64+dst] = (byte)(((b[ofs]&240) + (b[ofs+1]>>4))& 255); //b

			ofs+=2;
			dst++;
		}

		return converted;
	}
}
