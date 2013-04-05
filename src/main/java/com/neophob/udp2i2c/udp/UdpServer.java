package com.neophob.udp2i2c.udp;

import java.net.InetSocketAddress;

import org.jboss.netty.bootstrap.ConnectionlessBootstrap;
import org.jboss.netty.channel.ChannelPipeline;
import org.jboss.netty.channel.ChannelPipelineFactory;
import org.jboss.netty.channel.Channels;
import org.jboss.netty.channel.FixedReceiveBufferSizePredictorFactory;
import org.jboss.netty.channel.socket.DatagramChannelFactory;
import org.jboss.netty.channel.socket.nio.NioDatagramChannelFactory;

import com.neophob.udp2i2c.model.I2cConfig;

public class UdpServer {

	private final int port;
	private final I2cConfig i2cConfig;

    public UdpServer(int port, I2cConfig i2cConfig) {
        this.port = port;
        this.i2cConfig = i2cConfig;
    }

    public void run() throws Exception {
        DatagramChannelFactory f = new NioDatagramChannelFactory();
        ConnectionlessBootstrap b = new ConnectionlessBootstrap(f);

        // Configure the pipeline factory.
        b.setPipelineFactory(new ChannelPipelineFactory() {
            public ChannelPipeline getPipeline() throws Exception {
                return Channels.pipeline(
//                        new StringEncoder(CharsetUtil.ISO_8859_1),
//                        new StringDecoder(CharsetUtil.ISO_8859_1),
                        new UdpServerHandler(i2cConfig));
            }
        });
    	
        // Enable broadcast
        b.setOption("broadcast", "false");
        
        // Allow packets as large as up to 1024 bytes (default is 768).
        // You could increase or decrease this value to avoid truncated packets
        // or to improve memory footprint respectively.
        //
        // Please also note that a large UDP packet might be truncated or
        // dropped by your router no matter how you configured this option.
        // In UDP, a packet is truncated or dropped if it is larger than a
        // certain size, depending on router configuration.  IPv4 routers
        // truncate and IPv6 routers drop a large packet.  That's why it is
        // safe to send small packets in UDP.
        b.setOption(
                "receiveBufferSizePredictorFactory",
                new FixedReceiveBufferSizePredictorFactory(1024));
        
        // Bind to the port and start the service.
        b.bind(new InetSocketAddress(port));
    }


}