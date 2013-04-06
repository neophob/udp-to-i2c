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
package com.neophob.udp2i2c.rainbowduino;


/**
 * Various Helper Methods
 * <br><br>
 * part of the neorainbowduino library
 * 
 * @author Michael Vogt / neophob.com
 *
 */
public class RainbowduinoHelper {

	/**
	 * 
	 */
	private RainbowduinoHelper() {
		//no instance allowed
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
