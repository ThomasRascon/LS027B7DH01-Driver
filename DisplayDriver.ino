void setup() {
  // put your setup code here, to run once:
  // LCD commands - Note: the bits are reversed per the memory LCD data 
  // sheets because of the order the bits are shifted out in the SPI 
  // port. 
  #define MLCD_WR 0x80 //MLCD write line command 
  #define MLCD_CM 0x20 //MLCD clear memory command 
  #define MLCD_NO 0x00 //MLCD NOP command (used to switch VCOM) 
  //LCD resolution 
  #define MLCD_XRES 400 //pixels per horizontal line 
  #define MLCD_YRES 240 //pixels per vertical line 
  #define MLCD_BYTES_LINE MLCD_XRES / 8 //number of bytes in a line 
  #define MLCD_BUF_SIZE MLCD_YRES * MLCD_BYTES_LINE 
  //defines the VCOM bit in the command word that goes to the LCD 
  #define VCOM_HI 0x40 
  #define VCOM_LO 0x00  
  static char *frmbufter; //current address of buffer to be displayed 
  static char locbuf[MLCD_BYTES_LINE + 3]; //local line buffer 
  static char linenum; //current line number being transmitted 
  static int stage = 0; //there are 3 stages in transmitting a buffer: 
  //stage 0: first line (has command in it) 
  //stage 1: 2nd through last line (no command) 
  //stage 2: null byte trailer 
  extern char vcom;  
  //current state of vcom. This should alternate 
  //between VCOM _ HI and VCOM _ LO on a 1-30 second 
  //period. 
}

void loop() {
  checkButton();
  toggleVCOM();

}
