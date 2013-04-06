package com.neophob.udp2i2c.stat;

import java.util.concurrent.atomic.AtomicLong;


/**
 * singleton to track statistic, thread safe
 * 
 * @author michu
 *
 */
public enum StatisticHelper {

	INSTANCE;

	private AtomicLong packetsRecieved = new AtomicLong();
	private AtomicLong errors = new AtomicLong();
	
	public long incrementAndGetPacketsRecieved() {
		return packetsRecieved.incrementAndGet();
	}

	public long incrementAndGetError() {
		return errors.incrementAndGet();
	}
	
	public long getErrorCount() {
		return errors.get();
	}
	

}
