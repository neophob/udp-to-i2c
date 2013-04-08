package com.neophob.udp2i2c;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;

public class I2cTrafficGenerator {

	private static final String VERSION = "0.1";
	
	private static void displayHelp() {
        System.out.println("Usage:\t\tTrafficGenerator [-b bus] [-t i2c_targets] [-s payload_size] [-l loops]\n");
		System.out.println("Example:\tTrafficGenerator -b 1 -t 4 -s 128 -l 3");
		System.out.println("\t");
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception {
		System.out.println("I2c traffic generator v"+VERSION+" by Michael Vogt / neophob.com");
		System.out.println("Send payload to the i2c bus");

        if (args.length < 4) {
        	displayHelp();
        	System.exit(1);
        }

        int target = -1;
        int bus = -1;        
        int size = -1;        
        int loop = -1;        
        
        int i=0;
        while (i < args.length && args[i].startsWith("-")) {
        	String arg = args[i++];
        	
        	if (arg.equals("-b")) {
        		if (i < args.length) { 
        			if (Integer.parseInt(args[i++])==0) {
        				bus = I2CBus.BUS_0;
        			} else {
        				bus = I2CBus.BUS_1;
        			}
        		} else {
                    System.err.println("-bus requires an integer value");
        		}
        	}

        	if (arg.equals("-t")) {
        		if (i < args.length) { 
        			target = Integer.parseInt(args[i++]);
        		} else {
                    System.err.println("-target requires an integer value");
        		}
        	}

        	if (arg.equals("-s")) {
        		if (i < args.length) { 
        			size = Integer.parseInt(args[i++]);
        		} else {
                    System.err.println("-size requires an integer value");
        		}
        	}

        	if (arg.equals("-l")) {
        		if (i < args.length) { 
        			loop = Integer.parseInt(args[i++]);
        		} else {
                    System.err.println("-loops requires an integer value");
        		}
        	}
        	
        }
        
        if (loop==-1 || size==-1 || target==-1 || bus==-1) {
        	displayHelp();
        	System.exit(1);        	
        }
        
        System.out.println("Sending "+size+" bytes on i2c bus "+bus+" to target address: "+target+", loops: "+loop);
        
        final I2CBus i2cBus = I2CFactory.getInstance(bus);
        I2CDevice arduino = i2cBus.getDevice(target);
        byte[] buffer = new byte[size];
        for (i=0; i<buffer.length; i++) {
        	buffer[i] = (byte)i;
        }
        
        for (i=0; i<loop; i++) {
            System.out.println("send buffer now");
            
            long l = System.currentTimeMillis();
            arduino.write(buffer, 0, buffer.length);
            long needed = System.currentTimeMillis() - l;
            System.out.println("done in "+needed+"ms");        	
        }        
	}

}
