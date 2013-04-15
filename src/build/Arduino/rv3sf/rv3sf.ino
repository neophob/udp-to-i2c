/*
Rainbowduino V3 firmware capable of streaming 24bit RGB frames with up
to 35fps via the USB connector of the Rainbowduino V3 controller.

Author:   Markus Lang (m@rkus-lang.de)
Websites: http://programmers-pain.de/
          https://code.google.com/p/rainbowduino-v3-streaming-firmware/
          
This firmware is based on several Rainbowduino related firmwares like:
neorainbowduino:  http://code.google.com/p/neorainbowduino/
rainbowdash:      http://code.google.com/p/rainbowdash/
seeedstudio.com:  http://www.seeedstudio.com/wiki/Rainbowduino_v3.0

The Java part splits a full 8x8 RGB frame (3 colors * 64 LEDs = 192byte)
into four frame fragments (each 48byte) to get around the 64byte default
buffer size of the Arduino hardware serial implementation. Each fragment
will be extended with the HEADER bits and a frame fragment index to
be able to reconstruct the full frame in the correct order inside this
firmware. 

Splitting up the frame into fragments avoids running into data corruption
/ data loss if the Java part sends more bytes than the Arduino controller
can buffer. For every frame fragment the controller will send an ACK REPLY
message to the Java code so that the next fragment will be send.

The firmware is able to handle incomplete frames as well as CRC checksum
errors of the transferred LED color data so that it's able to signal those
error conditions to the Java API.

The LED update routine of this firmware is just a rewrite of the original
seeedstudio.com firmware (http://www.seeedstudio.com/wiki/Rainbowduino_v3.0)
including some changes regarding the interrupt handling to allow the
controller to update the LEDs and receiving incoming serial data at the
same time.

Hints:
https://github.com/sysrun/GroveLedBar/blob/master/GroveLedbar.cpp

Open Issues: 
-why do we need an interupt routine?

*/

#include <Wire.h>

// ports and bit values needed by the LED update routine
#define DDR_DATA   DDRB
#define DDR_CLK    DDRB
#define DDR_LINES  DDRD
#define PORT_DATA  PORTB
#define PORT_CLK   PORTB
#define PORT_LINES PORTD
#define BIT_DATA   0x01
#define BIT_CLK    0x02
#define BIT_LINES  0xF0

//32 byte per color
#define DATA_LEN_4_BIT 96
const unsigned char I2C_ADDRESS = 4;

// general const variables
const unsigned char RAINBOWDUINO_LEDS = 64;
const unsigned char NUMBER_OF_COLORS = 3;
const unsigned char RED = 0;
const unsigned char GREEN = 1;
const unsigned char BLUE = 2;

// byte array used as frame buffers for the currently received and displayed frame
// (2 buffers, RGB colors, 8 rows, 8 columns)
unsigned char frameBuffers[2][NUMBER_OF_COLORS][8][8];
// the currently used frame buffer by the LED update routine
volatile byte currentFrameBuffer;
volatile byte switchFramebuffer;
// the current line the LED update routine will push color data for
volatile byte currentLine;

void setup() {
  // initialize global variables used to update the LEDs
  currentFrameBuffer = 0;
  currentLine = 0;
  switchFramebuffer = 0;

  // disable all internal interrupts
  cli();
  
  // initialize LED update routine and MY9221 state
  DDR_LINES  |=  BIT_LINES;
  PORT_LINES &= ~BIT_LINES;
  DDRD |= 0x04;
  DDR_DATA  |=  BIT_DATA;
  DDR_CLK   |=  BIT_CLK;
  PORT_DATA &= ~BIT_DATA;
  PORT_CLK  &= ~BIT_CLK;
  DDRB |= 0x20;
  // clear the display to get a clean state
  clearDisplay();
  
  // init TIMER 1 (trigger every ~1250us)
  TCCR1A = 0;
  TCCR1B = _BV(WGM13);
  ICR1 = 10000;
  TIMSK1 = _BV(TOIE1);
  TCNT1 = 0;
  TCCR1B |= _BV(CS10);
  // re-enable all internal interrupts
  sei();
  
  //initial image: i like yellow...
  for (byte b=0; b<8; b++) {  
    for (byte row=0; row<8; row++) {
      frameBuffers[0][RED][row][b] = 255;
      frameBuffers[1][BLUE][row][b] = 255;    
      //frameBuffers[0][GREEN][row][b] = 255;
      //frameBuffers[1][GREEN][row][b] = 255;    
    }
  }
  
  Wire.begin(I2C_ADDRESS);                // join i2c bus with address #4
  //TODO really not needed here????
  Wire.onReceive(receiveEvent); // register event
  
}


//get data from master - HINT: this is a ISR call!
//HINT2: do not handle stuff here!! this will NOT work
//collect only data here and process it in the main loop!
void receiveEvent(int howMany) {
}

void checkForNewFrames() {
  byte b=0;
  byte dataSize = Wire.available();
  if (dataSize>=DATA_LEN_4_BIT) {
    //read image data (payload) - an image size is exactly 96 bytes
    //frameBuffers[currentFrameBuffer][currentColor][currentRow][currentColumn] = serialData;
    byte col0,col1;
    byte fbNotInUse = !currentFrameBuffer;
    
    byte row=0;
    byte col=0;
    for (b=0; b<32; b++) {
      col1 = Wire.read();
      col0 = col1 >> 4;
      col1 &= 0x0f;
      frameBuffers[fbNotInUse][RED][row][col++] = col0;
      frameBuffers[fbNotInUse][RED][row][col++] = col1;
      if (col==7) {
        col=0;
        row++;
      }
    }    
    
    col=0;
    row=0;
    for (b=0; b<32; b++) {
      col1 = Wire.read();
      col0 = col1 >> 4;
      col1 &= 0x0f;
      frameBuffers[fbNotInUse][GREEN][row][col++] = col0;
      frameBuffers[fbNotInUse][GREEN][row][col++] = col1;
      if (col==7) {
        col=0;
        row++;
      }
    }
    
    col=0;
    row=0;
    for (b=0; b<32; b++) {
      col1 = Wire.read();
      col0 = col1 >> 4;
      col1 &= 0x0f;
      frameBuffers[fbNotInUse][BLUE][row][col++] = col0;
      frameBuffers[fbNotInUse][BLUE][row][col++] = col1;
      if (col==7) {
        col=0;
        row++;
      }
    }
    
    switchFramebuffer = 1;
    
  }
}

int cnt=0;
void loop() {

  checkForNewFrames();

/*  
  cnt++;
  if (cnt > 10000) {
  //used to debug framw switching
    switchFramebuffer= 1;
    cnt=0;
  }
*/
}


void send16BitData(unsigned int data) {
  for (byte i = 0; i < 16; i++) {
    if (data & 0x8000) {
      PORT_DATA |=  BIT_DATA;
    } else {
      PORT_DATA &= ~BIT_DATA;
    }
    PORT_CLK ^= BIT_CLK;
    data <<= 1;
  }
}

void latchData() {
  PORT_DATA &= ~BIT_DATA;
  delayMicroseconds(10);
  PORT_LINES &= ~0x80;
  for (unsigned char i = 0; i < 8; i++) {
    PORT_DATA ^= BIT_DATA;
  }
}

void switchOnDrive(unsigned char line) {
  PORT_LINES &= ~BIT_LINES;
  PORT_LINES |= (line << 4);
  PORT_LINES |= 0x80;
}

void clearData() {
  PORT_DATA &= ~BIT_DATA;
  for (byte i = 0; i < 192; i++) {
    PORT_CLK ^= BIT_CLK;
  }
}

void clearDisplay() {
  send16BitData(0);
  clearData();
  send16BitData(0);
  clearData();
  latchData();
}

ISR(TIMER1_OVF_vect) {
  // re-enable global interrupts, this needs some explanation:
  // ---------------------------------------------------------
  // to allow the internal interrupt of the Arduino framework to handle
  // incoming serial data we need to re-enable the global interrupts
  // inside this interrupt call of the LED update routine.
  // usually that's an stupid idea since the LED update rountine interrupt
  // could also be called a second time while this interrupt call is still
  // running - this would result in a hanging controller.
  // since we know that the interrupt is called in a 1250us interval and
  // the code in this method takes around ~650us to finish we still have
  // enough buffer to allow the internal interrupt to handle incoming serial
  // data without risking to block the controller. the time between the next
  // LED update rountine call is also sufficient to give the controller
  // enough time to parse the incoming serial data.
  // this setup was the easiest way out of the problem that the internal
  // interrupt and the interrupt of the LED update routine do otherwise
  // result in major data loss and data corruption if we wouldn't re-enable
  // the global interrupts here.
  
  //TODO really needed here????
  sei();
  // determine the frame buffer row to be used for this interrupt call
  byte row = 7 - currentLine;
  // clear the data of the former interrupt call to avoid flickering
  clearDisplay();
  // push data to the MY9221 ICs
  send16BitData(0);
  // push the blue color value of the current row
  for (byte column = 0; column < 8; column++) {
    send16BitData(frameBuffers[currentFrameBuffer][BLUE][row][column]);
  }
  // push the green color value of the current row
  for (byte column = 0; column < 4; column++) {
    send16BitData(frameBuffers[currentFrameBuffer][GREEN][row][column]);
  }
  send16BitData(0);
  for (byte column = 4; column < 8; column++) {
    send16BitData(frameBuffers[currentFrameBuffer][GREEN][row][column]);
  }
  // push the red color value of the current row
  for (byte column = 0; column < 8; column++) {
    send16BitData(frameBuffers[currentFrameBuffer][RED][row][column]);
  }
  // since the following code is timing-sensitive we have to disable
  // the global interrupts again to avoid ghosting / flickering of 
  // the other lines that shouldn't be active at all.

  //TODO really needed here????  
  cli();

  latchData();
  // activate current line
  switchOnDrive(currentLine);
  PORTD &= ~0x04;
  // increment current led row counter for the next interrupt call
  currentLine++;
  if (currentLine == 8) {
    currentLine = 0;
    
    if (switchFramebuffer==1) {
      switchFramebuffer = 0;
      currentFrameBuffer = !currentFrameBuffer;
    }

  }
}

