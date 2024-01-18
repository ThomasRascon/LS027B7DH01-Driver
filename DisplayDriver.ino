#include <SPI.h>

/*
 * Macros
 */
#define MLCD_WR 0x01 //MLCD write line command
#define MLCD_CM 0x04 //MLCD clear memory command
#define MLCD_NO 0x00 //MLCD NOP command (used to switch VCOM)
//LCD resolution
#define MLCD_XRES 128 //pixels per horizontal line
#define MLCD_YRES 128 //pixels per vertical line
#define MLCD_BYTES_LINE MLCD_XRES / 8 //number of bytes in a line
#define MLCD_BUF_SIZE MLCD_YRES * MLCD_BYTES_LINE
//defines the VCOM bit in the command word that goes to the LCD
#define VCOM_HI 0x02
#define VCOM_LO 0x00  
#define NUM_IMGS 2
//Definition of pins
#define DISP 11
#define SCS 10

/*
 * Custom spi settings
 */
SPISettings spisettings(1000000, LSBFIRST, SPI_MODE0);

/*
 * Gobal variable declaration
 */
static char lineBuff[MLCD_BYTES_LINE+3];
static int img_num = 1;
static char vcom = 0x00;
static bool power = false;
static char *frmBuff;
static char *firstBuffByte;


void setup() {   
    SPI.setSCK(2);
    SPI.setTX(3);
    SPI.begin(true);
    
    frmBuff = (char*)malloc(NUM_IMGS * MLCD_YRES * MLCD_BYTES_LINE * sizeof(char));
    if(frmBuff == NULL){
      printf("Memory allocation failed!\n");
    }
    firstBuffByte = frmBuff;
    checkerPattern();
    powerOn();
}

void powerOn(){
  power = false;
  delay(100);
  digitalWrite(SCS, HIGH);
  SPI.beginTransaction(spisettings);
  SPI.transfer(MLCD_CM);
  SPI.transfer(MLCD_NO);
  SPI.endTransaction();
  digitalWrite(SCS, LOW);
  delay(1);
  digitalWrite(DISP, HIGH);
}


void checkButton(){
  if(BOOTSEL){
    img_num += 1;
  }
  int it = 0;
  while(BOOTSEL){
    delay(100);
    it++;
    if(it >= 10){
      if(power){
        powerOff();
        img_num -= 1;
      } else {
        powerOn();
        img_num -= 1;
      }
    }
  }
  if(img_num > NUM_IMGS){
    img_num = 1;
  }
}


void powerOff(){
  power = false;
  digitalWrite(SCS, HIGH);
  SPI.beginTransaction(spisettings);
  SPI.transfer(MLCD_CM);
  SPI.transfer(MLCD_NO);
  SPI.endTransaction();
  digitalWrite(SCS, LOW);
  delay(100);
  digitalWrite(DISP, LOW);
}


void writeBuffer(){
  digitalWrite(SCS, HIGH);
  char* locLineBuff = lineBuff;
  char lineAddress = 1; 


  *locLineBuff++ = MLCD_WR || vcom;
  while(lineAddress <= MLCD_YRES){
    *locLineBuff++ = lineAddress++;
    for(int i = 0; i < MLCD_BYTES_LINE; ++i){
      *locLineBuff++ = *frmBuff++;
    }
    *locLineBuff++ = 0;
    SPI.beginTransaction(spisettings);
    SPI.transfer(lineBuff, locLineBuff - lineBuff);
    SPI.endTransaction();
    locLineBuff = lineBuff;
  }
  if(img_num == NUM_IMGS){
    frmBuff = firstBuffByte;
  }
  SPI.beginTransaction(spisettings);
  SPI.transfer(MLCD_NO);
  SPI.endTransaction();
  digitalWrite(SCS, LOW);
}


void toggleVcom(){
  vcom = vcom ^ VCOM_HI;
  digitalWrite(SCS, HIGH);
  SPI.beginTransaction(spisettings);
  SPI.transfer(&vcom, 1);
  SPI.transfer(MLCD_NO, 1);
  SPI.endTransaction();
  digitalWrite(SCS, LOW);
}


void checkerPattern(){
    char* locBuff = frmBuff;
    for(int i = 0; i < MLCD_YRES * MLCD_BYTES_LINE; ++i){
        *locBuff++ = 0x80;
    }
    for(int i = 0; i < MLCD_YRES * MLCD_BYTES_LINE; ++i){
        *locBuff++ = 0x08;
    }
}


void loop() {
    int prev_img = img_num;
    checkButton();
    if(img_num != prev_img){
      writeBuffer();
    }
}
