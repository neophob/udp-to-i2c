static void isr2() {
  // re-enable global interrupts, this needs some explanation:
  // ---------------------------------------------------------
  // to allow the internal interrupt of the Arduino framework to handle
  // incoming i2c data we need to re-enable the global interrupts
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
  sei();

  //increment line counter
  currentLine++;

  if (currentLine > 7) {
    currentLine = 0;

    if (switchFramebuffer==1) {
      switchFramebuffer = 0;
      currentFrameBuffer = !currentFrameBuffer;
    }
  }
  
  byte ofs = currentLine<<2;

  // clear the data of the former interrupt call to avoid flickering
  //use alot of time!
  clearDisplay();

  // push data to the MY9221 ICs
  send16BlanksWithoutDataLineInit();

  // push the blue color value of the current row
  send32BitData(frameBuffers[currentFrameBuffer][BLUE][ofs+0]);
  send32BitData(frameBuffers[currentFrameBuffer][BLUE][ofs+1]);
  send32BitData(frameBuffers[currentFrameBuffer][BLUE][ofs+2]);
  send32BitData(frameBuffers[currentFrameBuffer][BLUE][ofs+3]);

  // push the green color value of the current row
  send32BitData(frameBuffers[currentFrameBuffer][GREEN][ofs+0]);
  send32BitData(frameBuffers[currentFrameBuffer][GREEN][ofs+1]);

  send16Blanks();

  send32BitData(frameBuffers[currentFrameBuffer][GREEN][ofs+2]);
  send32BitData(frameBuffers[currentFrameBuffer][GREEN][ofs+3]);

  // push the red color value of the current row
  send32BitData(frameBuffers[currentFrameBuffer][RED][ofs+0]);  
  send32BitData(frameBuffers[currentFrameBuffer][RED][ofs+1]);    
  send32BitData(frameBuffers[currentFrameBuffer][RED][ofs+2]);  
  send32BitData(frameBuffers[currentFrameBuffer][RED][ofs+3]);    

  // since the following code is timing-sensitive we have to disable
  // the global interrupts again to avoid ghosting / flickering of 
  // the other lines that shouldn't be active at all.
  cli(); //disable interrupt

  //--- latchData(); ----
//  PORT_DATA &= ~BIT_DATA;
  //6ms - not working, 8ms also buggy
  delayMicroseconds(DELAY_PER_LINE);

  //At 16Mhz, it takes 1us to execute 16 nop instructions
  //asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
  //asm("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");


  PORT_LINES &= ~0x80;
  /*  for (unsigned char i = 0; i < 8; i++) {
   PORT_DATA ^= BIT_DATA;
   }*/
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;

  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;
  PORT_DATA ^= BIT_DATA;

  // ------------------------------------------------------
  // activate current line
  //  switchOnDrive(currentLine++);
  PORT_LINES &= ~BIT_LINES;
  PORT_LINES |= (currentLine << 4);
  PORT_LINES |= 0x80;

  PORTD &= ~0x04;
}

