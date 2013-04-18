/*
Rainbowduino V3 firmware 
recieve data via i2c bus (96 bytes per frame) and display result on screen
 
origianl source by 
 Author:   Markus Lang (m@rkus-lang.de)
 Websites: http://programmers-pain.de/
 https://code.google.com/p/rainbowduino-v3-streaming-firmware/
 
heavy optimized and changed by Michael Vogt (michu@neophob.com)
 Website: http://www.pixelinvaders.ch
 
 */
#include <TimerOne.h>
#include <Wire.h>

// ports and bit values needed by the LED update routine

//DDRB - The Port B Data Direction Register - read/write
#define DDR_DATA   DDRB
#define DDR_CLK    DDRB

//DDRD - The Port D Data Direction Register - read/write
#define DDR_LINES  DDRD

//PORTB maps to Arduino digital pins 8 to 13 The two high bits (6 & 7) map to the crystal pins and are not usable
#define PORT_DATA  PORTB
#define PORT_CLK   PORTB

//PORTD maps to Arduino digital pins 0 to 7
#define PORT_LINES PORTD

// 0000 0001
#define BIT_DATA   0x01

// 0000 0010
#define BIT_CLK    0x02

// 1111 0000
#define BIT_LINES  0xF0

//32 byte per color
#define DATA_LEN_4_BIT 96
#define I2C_ADDRESS 6

#define WAIT_MS_FOR_NEXT_FRAME 35
unsigned long time;

#define DELAY_PER_LINE 10
#define ISR_CALL_TIME 1490

//#define DEBUG 1
//#define POLICE_ANIMATION 1

// general const variables
const unsigned char RAINBOWDUINO_LEDS = 64;
const unsigned char NUMBER_OF_COLORS = 3;
const unsigned char RED = 0;
const unsigned char GREEN = 1;
const unsigned char BLUE = 2;

// byte array used as frame buffers for the currently received and displayed frame
// (2 buffers, RGB colors, 8 rows, 8 columns)
byte frameBuffers[2][NUMBER_OF_COLORS][32];
// the currently used frame buffer by the LED update routine
volatile byte currentFrameBuffer;
volatile byte switchFramebuffer;

// the current line the LED update routine will push color data for
byte currentLine;

// --------------------------------------------------------------------------------
void setup() {
  // initialize global variables used to update the LEDs
  currentFrameBuffer = 0;
  currentLine = 0;
  switchFramebuffer = 0;

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

  // re-enable all internal interrupts
  sei();

#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("hi");
#endif
  

  byte b;
  //initial image: remember, one byte store two colors
  for (b=0; b<32; b++) {  
      frameBuffers[0][RED][b] = 0x77;
      frameBuffers[0][BLUE][b] = 0x77;
      frameBuffers[1][RED][b] = 0x77;    
      frameBuffers[1][BLUE][b] = 0x77;    
  }

  //indicate i2c address
  for (b=0; b<I2C_ADDRESS/2; b++) {
      frameBuffers[0][GREEN][b] = 255;
      frameBuffers[1][GREEN][b] = 255;    
  }
  
  float f = I2C_ADDRESS/(float)2;
  int i = I2C_ADDRESS/2;  
  if (f>i) {
      frameBuffers[0][GREEN][b] = 240;
      frameBuffers[1][GREEN][b] = 240;    
  }

  Wire.begin(I2C_ADDRESS);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent);           // register event  

  Timer1.attachInterrupt(isr2);           // Start ISR routine
  //1730 is the minimum
  //2400 flicker (top down)
  //2500 flicker on one line
  //2600 flicker (down up)
  Timer1.initialize(ISR_CALL_TIME);

}


//get data from master - HINT: this is a ISR call! do not handle stuff here!! this will NOT work
//collect only data here and process it in the main loop!
void receiveEvent(int howMany) {
/*    if (howMany >= DATA_LEN_4_BIT) {
      checkForNewFrames();
    }*/
}

void checkForNewFrames() {
  while (switchFramebuffer==1) {
    //block until blit is done
  }

  byte b;
  //read image data (payload) - an image size is exactly 96 bytes
  //frameBuffers[currentFrameBuffer][currentColor][currentRow][currentColumn] = serialData;

  byte fbNotInUse = !currentFrameBuffer;
  byte col=0;
  for (b=0; b<32; b++) {
    frameBuffers[fbNotInUse][RED][col++] = Wire.read();
  }    

  col=0;
  for (b=0; b<32; b++) {
    frameBuffers[fbNotInUse][GREEN][col++] = Wire.read();
  }

  col=0;
  for (b=0; b<32; b++) {
    frameBuffers[fbNotInUse][BLUE][col++] = Wire.read();
  }

  switchFramebuffer = 1;
}


#ifdef POLICE_ANIMATION
int cnt;
#endif

#ifdef DEBUG
long msIrq = ISR_CALL_TIME;
#endif


// --------------------------------------------------------------------------------

void loop() {

  //limit framerate
  if (millis()-time < WAIT_MS_FOR_NEXT_FRAME) {
    return;
  }  
  time = millis();

  byte b = Wire.available();
  if (b>=DATA_LEN_4_BIT) { 
    checkForNewFrames();
  }

#ifdef DEBUG
  if (Serial.available()) {
    char in = Serial.read();
    if (in=='-') {
      msIrq-=10;
    }
    if (in=='+') {
      msIrq+=10;
    }
    if (in=='b') {
      msIrq-=1;
    }
    if (in=='a') {
      msIrq+=1;
    }
    Timer1.stop();

    Serial.print(msIrq);
    Serial.println(" speed");
    Timer1.setPeriod(msIrq);
    Timer1.resume();
  }
#endif

#ifdef POLICE_ANIMATION   
  cnt++;
  if (cnt > 4) {
    //used to debug framw switching
    switchFramebuffer= 1;
    cnt=0;
  }
#endif

}


void send16BitData(byte data) {
/*  for (byte i = 0; i < 16; i++) {
    if (data & 0x8000) {
      PORT_DATA |=  BIT_DATA;
    } 
    else {
      PORT_DATA &= ~BIT_DATA;
    }
    PORT_CLK ^= BIT_CLK;
    data <<= 1;
  }*/

  PORT_DATA &= ~BIT_DATA;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  //only 4 bytes are relevant
  for (byte i = 0; i < 4; i++) {
    if (data & 0x8) {
      PORT_DATA |=  BIT_DATA;
    } 
    else {
      PORT_DATA &= ~BIT_DATA;
    }
    PORT_CLK ^= BIT_CLK;
    data <<= 1;
  }
  
  //lower 4 bytes are low
  PORT_DATA &= ~BIT_DATA;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
}


void send32BitData(byte data) {
/*  for (byte i = 0; i < 16; i++) {
    if (data & 0x8000) {
      PORT_DATA |=  BIT_DATA;
    } 
    else {
      PORT_DATA &= ~BIT_DATA;
    }
    PORT_CLK ^= BIT_CLK;
    data <<= 1;
  }*/

  //first 8 bit's are always zero...
  PORT_DATA &= ~BIT_DATA;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  //only 4 bytes are relevant
  for (byte i = 0; i < 4; i++) {
    if (data & 0x80) {
      PORT_DATA |=  BIT_DATA;
    } 
    else {
      PORT_DATA &= ~BIT_DATA;
    }
    PORT_CLK ^= BIT_CLK;
    data <<= 1;
  }
  
  //lower 4 bytes are low
  PORT_DATA &= ~BIT_DATA;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  
  // -- second word send out here --
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  for (byte i = 0; i < 4; i++) {
    if (data & 0x80) {
      PORT_DATA |=  BIT_DATA;
    } 
    else {
      PORT_DATA &= ~BIT_DATA;
    }
    PORT_CLK ^= BIT_CLK;
    data <<= 1;
  }
  
  //lower 4 bytes are low
  PORT_DATA &= ~BIT_DATA;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;  
}


void send16Blanks() {
  PORT_DATA &= ~BIT_DATA;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
}

//if the data line is already zero, call this
void send16BlanksWithoutDataLineInit() {
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
  PORT_CLK ^= BIT_CLK;
}


void latchData() {
  PORT_DATA &= ~BIT_DATA;
  //6ms - not working, 8ms also buggy
  delayMicroseconds(DELAY_PER_LINE);

  //At 16Mhz, it takes 1us to execute 16 nop instructions
  //asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
  //asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");

  PORT_LINES &= ~0x80;

  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;

  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
}

/*void switchOnDrive(unsigned char line) {
 PORT_LINES &= ~BIT_LINES;
 PORT_LINES |= (line << 4);
 PORT_LINES |= 0x80;
 }*/

void clearData() {
  PORT_DATA &= ~BIT_DATA;
  for (byte i = 0; i < 192; i++) {
    PORT_CLK ^= BIT_CLK;
  }
}


void clearDisplay() {
  /*  send16BitData(0);
   clearData();
   send16BitData(0);
   clearData();
   latchData();
   */

  PORT_DATA &= ~BIT_DATA;
  for (int i = 0; i < 416; i++) {
    PORT_CLK ^= BIT_CLK;
  }

  //latch data
  delayMicroseconds(DELAY_PER_LINE);
  PORT_LINES &= ~0x80;

  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;

  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
}


