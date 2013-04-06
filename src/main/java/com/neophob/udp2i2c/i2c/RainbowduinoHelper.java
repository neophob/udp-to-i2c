/**
 * Copyright (C) 2011-2013 Michael Vogt <michu@neophob.com>
 *
 * This file is part of PixelController.
 *
 * PixelController is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PixelController is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PixelController.  If not, see <http://www.gnu.org/licenses/>.
 */
package com.neophob.udp2i2c.i2c;


/**
 * Various Helper Methods
 * <br><br>
 * part of the neorainbowduino library
 * 
 * @author Michael Vogt / neophob.com
 *
 */
public class RainbowduinoHelper {

	private static final int BUFFERSIZE = 8*8;

	/**
	 * 
	 */
	private RainbowduinoHelper() {
		//no instance allowed
	}
	

	/**
	 * convert rgb image data to rainbowduino compatible format
	 * format 8x8x4
	 * 
	 * @param data the rgb image as int[64] 192 bytes
	 * @return rainbowduino compatible format as byte[3*8*4] 
	 */
	public static byte[] convertRgbToRainbowduino(byte[] data) throws IllegalArgumentException {
		if (data==null) {
			throw new IllegalArgumentException("data is null!");
		}

		byte[] converted = new byte[3*8*8];
		int[] r = new int[BUFFERSIZE];
		int[] g = new int[BUFFERSIZE];
		int[] b = new int[BUFFERSIZE];
		int tmp;
		int ofs=0;
		int dst;

		//step#1: split up r/g/b and apply gammatab
		for (int n=0; n<BUFFERSIZE; n++) {
			//one int contains the rgb color
			tmp = data[ofs];

			//the buffer on the rainbowduino takes GRB, not RGB				
			g[ofs] = (int) ((tmp>>16) & 255);  //r
			r[ofs] = (int) ((tmp>>8)  & 255);  //g
			b[ofs] = (int) ( tmp      & 255);  //b		
			ofs++;
		}
		//step#2: convert 8bit to 4bit
		//Each color byte, aka two pixels side by side, gives you 4 bit brightness control, 
		//first 4 bits for the left pixel and the last 4 for the right pixel. 
		//-> this means a value from 0 (min) to 15 (max) is possible for each pixel 		
		ofs=0;
		dst=0;
		for (int i=0; i<32;i++) {
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
