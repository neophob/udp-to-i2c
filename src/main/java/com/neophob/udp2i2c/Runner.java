package com.neophob.udp2i2c;

import java.util.ArrayList;
import java.util.List;

import com.neophob.udp2i2c.model.I2cConfig;
import com.neophob.udp2i2c.udp.UdpServer;
import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CFactory;

public class Runner {

	private static final String VERSION = "0.1";
	
	private static void displayHelp() {
        System.out.println("Usage:\t\tRunner [-p port] [-b bus] [-d delay] [-t i2c_targets]\n");
		System.out.println("Example:\tRunner -p 65506 -b 1 -d 10 -t 4:5:6:7");
		System.out.println("\t");
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception {
		System.out.println("UDP-to-I2c Server v"+VERSION+" by Michael Vogt / neophob.com");
		System.out.println("Bridge a TPM2Net UDP packet to I2C");

        if (args.length < 3) {
        	displayHelp();
        	System.exit(1);
        }

        List<Integer> i2cAddress = new ArrayList<Integer>();
        int port = -1;
        int bus = -1;        
        int delay = -1;
        
        int i=0;
        while (i < args.length && args[i].startsWith("-")) {
        	String arg = args[i++];
        	
        	if (arg.equals("-p")) {
        		if (i < args.length) { 
        			port = Integer.parseInt(args[i++]);
        		} else {
                    System.err.println("-port requires an integer value");
        		}
        	}

        	if (arg.equals("-d")) {
        		if (i < args.length) { 
        			delay = Integer.parseInt(args[i++]);
        		} else {
                    System.err.println("-delay requires an integer value");
        		}
        	}

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
        			String targets = args[i++];
        			if (targets.contains(":")) {
        				String[] tmp = targets.split(":");
        				for (String s: tmp) {
        					i2cAddress.add(Integer.parseInt(s));
        				}
        			} else {
        				i2cAddress.add(Integer.parseInt(targets));
        			}
        		} else {
        			System.err.println("-t requires an integer value");
        		}
        	}
        }
        
        System.out.println("Listening on port "+port+", using i2c bus "+bus+", i2c target address:");
        System.out.print("\t");
        for (int a: i2cAddress) {
        	System.out.print(a+" ");
        }
        System.out.println();
        
        final I2CBus i2cBus = I2CFactory.getInstance(bus);
        
        I2cConfig i2cConfig = new I2cConfig(i2cBus, i2cAddress, delay);
        UdpServer srv = new UdpServer(port, i2cConfig);
        
        srv.run();
	}

}
