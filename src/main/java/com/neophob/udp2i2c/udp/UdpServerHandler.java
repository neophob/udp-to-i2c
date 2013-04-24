package com.neophob.udp2i2c.udp;

import org.jboss.netty.buffer.ChannelBuffer;
import org.jboss.netty.channel.ChannelHandlerContext;
import org.jboss.netty.channel.ExceptionEvent;
import org.jboss.netty.channel.MessageEvent;
import org.jboss.netty.channel.SimpleChannelUpstreamHandler;

import com.neophob.udp2i2c.i2c.I2cHelper;
import com.neophob.udp2i2c.model.I2cConfig;
import com.neophob.udp2i2c.stat.StatisticHelper;

public class UdpServerHandler extends SimpleChannelUpstreamHandler {

	private final I2cConfig i2cConfig;

	/**
	 * 
	 * @param i2cConfig
	 */
	public UdpServerHandler(I2cConfig i2cConfig) {
		this.i2cConfig = i2cConfig;
	}

	
	/**
	 * callback if one packet arrives. it contains data for one panel
	 */
	public void messageReceived(ChannelHandlerContext ctx, MessageEvent e) throws Exception {
		ChannelBuffer msg = (ChannelBuffer) e.getMessage();

		//read tpm2.net header
		byte tpm2netStart = msg.getByte(0);
		byte tpm2netCmd = msg.getByte(1);

		//check if header is valid
		if (tpm2netStart!=(byte)0x9c || tpm2netCmd!=(byte)0xda) {
			System.out.println("Error, invalid header data recieved! "+String.valueOf(tpm2netStart&255));
			return;
		}

//		short frameSize = msg.getShort(2);
		byte nr = msg.getByte(4);

		//subtract 6 bytes, 5 bytes header and 1 byte footer
		int length = msg.array().length-6;
		int i2cAddress=0;
		try {
			i2cAddress = i2cConfig.getI2cAddress().get(nr);
		} catch (java.lang.IndexOutOfBoundsException ex) {
			System.out.println("failed to get i2c for offset "+nr);
			return;
		}
//		System.out.println("framesize: "+frameSize+", i2c address: "+i2cAddress+", data per panel: "+length);

		byte[] buffer = new byte[length];
		System.arraycopy(msg.array(), 5, buffer, 0, length);
		try {
			I2cHelper.sendData(i2cConfig, i2cAddress, buffer);
			long cnt = StatisticHelper.INSTANCE.incrementAndGetPacketsRecieved();
			if (cnt%1000==0) {
				System.out.println("Packets recieved: "+cnt+", errors: "+StatisticHelper.INSTANCE.getErrorCount());
			}
		} catch (Exception ex) {
			StatisticHelper.INSTANCE.incrementAndGetError();
			System.err.println("Failed to send data to i2c address "+i2cAddress+"."+
					"Packets recieved: "+StatisticHelper.INSTANCE.getPacketCount()+", errors: "+StatisticHelper.INSTANCE.getErrorCount());
			ex.printStackTrace();
		}

	}

	
	@Override
	public void exceptionCaught(ChannelHandlerContext ctx, ExceptionEvent e) throws Exception {
		StatisticHelper.INSTANCE.incrementAndGetError();
		System.err.println("Ooops, error detected!");
		e.getCause().printStackTrace();
		// We don't close the channel because we can keep serving requests.
	}

}
