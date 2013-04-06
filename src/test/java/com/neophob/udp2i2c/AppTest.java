package com.neophob.udp2i2c;

import com.neophob.udp2i2c.i2c.I2cHelper;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

/**
 * Unit test for simple App.
 */
public class AppTest extends TestCase {
	
    /**
     * Create the test case
     *
     * @param testName name of the test case
     */
    public AppTest( String testName )
    {
        super( testName );
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite()
    {
        return new TestSuite( AppTest.class );
    }


    
    public void testConvert() throws Exception {
    	assertEquals(0, I2cHelper.convert24bitTo12bit(new byte[2]).length);
    	assertEquals(0, I2cHelper.convert24bitTo12bit(new byte[2000]).length);
    	assertEquals(0, I2cHelper.convert24bitTo12bit(new byte[0]).length);
    	
    	byte[] bfr = new byte[192];
    	for (int i=0; i<192; i++) {
    		bfr[i] = (byte)i;
    	}
    	byte[] ret = I2cHelper.convert24bitTo12bit(bfr);
    	
    	int i=0;
    	for (byte b: ret) {
    		System.out.print((int)(b&255)+",");
    		if (i++ % 8 ==7) {
    			System.out.println();
    		}
    	}
    	
    }
}
