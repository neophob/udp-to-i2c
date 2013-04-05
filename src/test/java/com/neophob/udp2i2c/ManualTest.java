package com.neophob.udp2i2c;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Arrays;

public class ManualTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception {
    	final String targetHost = "192.168.111.25";
    	final int targetPort = 6803;
    	
    	byte[] imageData = new byte[64];
    	Arrays.fill(imageData, (byte)99);
    	
        byte[] outputBuffer = new byte[64 + 5 + 1];
        
        //write header of udp packet
		outputBuffer[0] = (byte)0x9c;
		outputBuffer[1] = (byte)0xda;
		outputBuffer[2] = ((byte)(imageData.length >> 8 & 0xFF));
		outputBuffer[3] = ((byte)(imageData.length & 0xFF));
		outputBuffer[4] = ((byte)1);
		System.arraycopy(imageData, 0, outputBuffer, 5, imageData.length);		
    	
    	InetAddress address = InetAddress.getByName(targetHost);
		DatagramPacket packet = new DatagramPacket(new byte[0], 0, address, targetPort);
		DatagramSocket dsocket = new DatagramSocket();
    	
		packet.setData(outputBuffer);
		packet.setLength(outputBuffer.length);
		dsocket.send(packet);		
		
		System.out.println("done");

	}

}
