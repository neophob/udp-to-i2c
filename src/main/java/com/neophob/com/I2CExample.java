package com.neophob.com;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;


public class I2CExample {

    /**
     * @param args
     */
    public static void main(String[] args) throws Exception {
    	
    	int size = Integer.parseInt(args[0]);
    	int loops = Integer.parseInt(args[1]);
        System.out.println("Starting, i2c size: "+size+", loops: "+loops);
        
        System.out.println("get bus 1");
        // get I2C bus instance
        final I2CBus bus = I2CFactory.getInstance(I2CBus.BUS_1);
        
        System.out.println("get device with id 4");
        I2CDevice arduino = bus.getDevice(0x04);
        byte[] buffer = new byte[size];
        for (int i=0; i<buffer.length; i++) {
        	buffer[i] = (byte)i;
        }
        
        for (int i=0; i<loops; i++) {
            System.out.println("send buffer now");
            
            long l = System.currentTimeMillis();
            //write(int address, byte[] buffer, int offset, int size) throws IOException        
            arduino.write(buffer, 0, buffer.length);
            long needed = System.currentTimeMillis() - l;
            //arduino.write((byte)65);
            
            System.out.println("done in "+needed+"ms");        	
        }
    }
    
/*    public static class WiiMotionPlus {
        
        private I2CDevice initDevice;
        private I2CDevice device;
        
        public WiiMotionPlus(I2CBus bus) throws IOException {
            initDevice = bus.getDevice(0x53);
            device = bus.getDevice(0x52);
        }
        
        public void init() {
            try {
                initDevice.write(0xfe, (byte)0x04);
            } catch (IOException ignore) {
                ignore.printStackTrace();
            }
        }
        
        public ThreeAxis read() throws IOException {
            byte[] buf = new byte[256];
            int res = device.read(0, buf, 0, 6);

            if (res != 6) {
                throw new RuntimeException("Read failure - got only " + res + " bytes from WiiMotionPlus");
            }

            
            ThreeAxis ret = new ThreeAxis();
            
            ret.x = asInt(buf[0]);
            ret.y = asInt(buf[1]);
            ret.z = asInt(buf[2]);
            ret.x = ret.x | (((asInt(buf[3]) & 0xfc) >> 2) * 256);
            ret.y = ret.y | (((asInt(buf[4]) & 0xfc) >> 2) * 256);
            ret.z = ret.z | (((asInt(buf[5]) & 0xfc) >> 2) * 256);

            return ret;
        }

        private int asInt(byte b) {
            int i = b;
            if (i < 0) { i = i + 256; }
            return i;
        }
    
    }
    
    public static class ThreeAxis {
        
        public int x;
        public int y;
        public int z;
    }    */
}