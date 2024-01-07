#include <SPI.h>
SPISettings spisettings(1000000, LSBFIRST, SPI_MODE1)


void setup() {
    // put your setup code here, to run once:
    // LCD commands - Note: the bits are reversed per the memory LCD data
    // sheets because of the order the bits are shifted out in the SPI
    // port.
    #define MLCD_WR 0x80 //MLCD write line command
    #define MLCD_CM 0x20 //MLCD clear memory command
    #define MLCD_NO 0x00 //MLCD NOP command (used to switch VCOM)
    //LCD resolution
    #define MLCD_XRES 128 //pixels per horizontal line
    #define MLCD_YRES 128 //pixels per vertical line
    #define MLCD_BYTES_LINE MLCD_XRES / 8 //number of bytes in a line
    #define MLCD_BUF_SIZE MLCD_YRES * MLCD_BYTES_LINE
    //defines the VCOM bit in the command word that goes to the LCD
    #define VCOM_HI 0x40
    #define VCOM_LO 0x00  
    #define NUM_IMGS 2
    //Definition of pins
    #define DISP 4
    #define SCS 5
   
    SPI.setSCK(2);
    SPI.setTX(3);
    SPI.begin(true);


}
static char *frmbuff;
static int img_num = 0
extern char vcom = 0x00


void loop() {
    frmbuff = (char*)malloc(NUM_IMGS * MLCD_YRES * MLCD_BYTES_LINE * sizeof(char));
    checkerPattern();
    checkButton();
    if(img_num==1){
      powerOn();
    }
    if(img_num > NUM_IMGS){
      powerOff();
      img_num = 1;
    }
}


void powerOn(){
  delay(100);
  digitalWrite(SCS, HIGH);
  SPI.beginTransaction(spisettings);
  SPI.transfer(MLCD_CM, sizeof(MLCD_CM));
  SPI.transfer(MLCD_NO, sizeof(MLCD_NO));
  SPI.endTransaction();
  digitalWrite(SCS, LOW)
  delay(1);
  digitalWrite(DISP, HIGH);
}


void checkButton(){
  if(BOOTSEL){
    img_num += 1
  }
  int it = 0
  while(BOOTSEL){
    delay(100);
    it++;
    if(it >= 10){
      powerOff();
    }
  }
}


void powerOff(){
  digitalWrite(SCS, HIGH);
  SPI.beginTransaction(spisettings);
  SPI.transfer(MLCD_CM, sizeof(MLCD_CM));
  SPI.transfer(MLCD_NO, sizeof(MLCD_NO));
  SPI.endTransaction();
  digitalWrite(SCS, LOW)
  delay(100);
  digitalWrite(DISP, LOW);
}


void writeOneLine(){


}


void writeMultiple(){


}


void toggleVcom(){
  vcom = vcom ^ VCOM_HIGH;
  digitalWrite(SCS, HIGH);
  SPI.beginTransaction(spisettings);
  SPI.transfer(vcom, sizeof(vcom));
  SPI.transfer(MLCD_NO, sizeof(MLCD_NO));
  SPI.endTransaction();
  digitalWrite(SCS, LOW)
}


void checkerPattern(){
    char* locbuff = frmbuff;
    for(int i = 0; i < NUM_IMGS * MLCD_YRES * MLCD_BYTES_LINE; ++i){
        bool s1 = i%MLCD_BYTES_LINE < int(MLCD_BYTES_LINE/2);
        bool s2 = i < NUM_IMGS * MLCD_YRES * MLCD_BYTES_LINE;
        bool s3 = int(i/MLCD_BYTES_LINE) < 64;
        bool flag = (s1 ^ s2) ? s3 : !s3;
        if(flag){
            *locbuff++ = 0xFF;
        }
        else{
            *locbuff++ = 0x00;
        }
    }
}
