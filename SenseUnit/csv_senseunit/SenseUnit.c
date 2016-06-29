#include <stdlib.h>
#include "si4010.h"
#include "si4010_api_rom.h"
#include "si4010_api_add.h"
#include "si4010_types.h"
#include "i2c.h"

#define bFStepDemo_MaxFrameSize_c (125)
#define bFStepDemo_FreqNum_c (1)
#define INT_MIN -32767
#define INT_MAX 32767
#define PACKET_SIZE 12
#define THRESHHOLD 190
//#define THRESHHOLD 720
// sizeof(int) == 2
// sizeof(short) == 2
// sizeof(BYTE) == 1

//Types
typedef BYTE tFStepDemo_Data[bFStep_DataLength_c];

//Function Prototypes
tFStepDemo_Data xdata arFStepDemo_Data[bFStepDemo_FreqNum_c];
void Delay(void);
void voidit(int z);
void sendData(BYTE xdata * dataToSend, BYTE len);
void makePacket(BYTE xdata * dataBuffer, BYTE * temp, short payload_len);
void blink(short state, short numBlinks);

//Stop unused vars from blocking comp
void voidit(int z) {
  int q = z;
  return;
}

void main(void)
{
  unsigned char ack;
  BYTE xdata x[PACKET_SIZE];
  BYTE xdata z[PACKET_SIZE];
  BYTE xdata y[PACKET_SIZE];
  short xdata magx;
  short xdata magz;
  short xdata magy;
  BYTE incr = 0;
  BYTE temp[6];
  
  // --------- Detection Version 1 ---------
  int max = INT_MIN;
  int min = INT_MAX;
  int diff = 0;
  int mid = 0;
  int counter = 0;
  // ---------------------------------------

  // For testing purpose
  short xdata prev = 0;
  // Data order: magx magz magy
  
  tOds_Setup xdata rOdsSetup;
  tPa_Setup xdata  rPaSetup;
  //BYTE xdata    *pbFrameHead;

  // Disable the Matrix and Roff modes on GPIO[3:1]
  PORT_CTRL &= ~(M_PORT_MATRIX | M_PORT_ROFF | M_PORT_STROBE);
  PORT_CTRL |=  M_PORT_STROBE;
  PORT_CTRL &= (~M_PORT_STROBE);

  // Setup
  PDMD=1;
  vSys_Setup( 1 );
  vSys_BandGapLdo( 1 );
  rPaSetup.fAlpha      = 0;
  rPaSetup.fBeta       = 0;
  rPaSetup.bLevel      = 77; //77 is loud 20 is quiet
  rPaSetup.wNominalCap = 192;
  rPaSetup.bMaxDrv     = 1;
  vPa_Setup( &rPaSetup );
  
  rOdsSetup.bModulationType = 1;  // Use FSK
  rOdsSetup.bClkDiv         = 5;
  rOdsSetup.bEdgeRate       = 0;
  rOdsSetup.bGroupWidth     = 7;
  rOdsSetup.wBitRate        = 0x61B; //80    higher is slower
  rOdsSetup.bLcWarmInt  = 0; //0
  rOdsSetup.bDivWarmInt = 5; //5
  rOdsSetup.bPaWarmInt  = 4; //4
  vOds_Setup( &rOdsSetup );

  vStl_EncodeSetup( bEnc_Manchester_c, NULL );
  //vStl_EncodeSetup( bEnc_NoneNrz_c, NULL );
  //pbFrameHead = (BYTE xdata *) abFStepDemo_FrameArray;
  vFCast_Setup();

  //vFCast_Tune( 440.0e6 );
  vFCast_Tune( 440001227 );
  bFStep_Collect( (BYTE xdata *) &(arFStepDemo_Data[0]) );

  //I2C Stuff
  // Write to slave device
  I2CInit();
  I2CStart();
  ack = I2CSend(0x3C); //0x3C is mag addr
  ack = I2CSend(0x02); ///select mode register
  ack = I2CSend(0x00); ///continuous measurement
  I2CStop();
  
  GPIO1 = 1;
  
  if(PDMD != 500){
    GPIO1 = 1;
     I2CRestart();
    I2CNak();
    I2CRead();
    I2CAck();
    I2CStart();
    I2CStop();
    I2CInit();
    I2CSend(0x00);
  }
  
  Delay();
  while ( 1 )
  {
    // Read from slave device
  I2CStart();
   I2CSend(0x3C);
   I2CSend(0x03);
  I2CStop();
  
  I2CStart();
  I2CSend(0x3C | 1);
   //I2CSend(0x06);

  magx = I2CRead();
  I2CAck();  
  magx = magx << 8;
  magx |= I2CRead();
  I2CAck();

  magz = I2CRead();
  I2CAck();  
  magz = magz << 8;
  magz |= I2CRead();
  I2CAck();

  magy = I2CRead();
  I2CAck();  
  magy = magy << 8;
  magy |= I2CRead();
  I2CNak();

  I2CStop();
  
  Delay();

   // -------- DETECTION TEST ------------
   //blink( abs(magz - prev) > THRESHHOLD, 5 );
   //prev = magx;
   // TODO: Does I2C work properly?
   //       Observe the range of sensor data.
   //       Then we can come up with an algorithm 
   if (min > magz) {
      min = magz;
   }
   if (max < magz) {
      max = magz;
   }
   diff = abs(min - max);
   if (diff > THRESHHOLD) {
      max = INT_MIN; // or set it to some middle value
      min = INT_MAX; // or set it to some middle value
      blink(1, 5);
   }

   // ---------- DATA SEND -----------
   // GPIO: 1=off, 0=on
   temp[0] = (magx & 0xFF00) >> 8;
   temp[1] = magx & 0x00FF;
   temp[2] = (magz & 0xFF00) >> 8;
   temp[3] = magz & 0x00FF;
   temp[4] = (magy & 0xFF00) >> 8;
   temp[5] = magy & 0x00FF;
   
   makePacket(x, temp, PACKET_SIZE);
   incr++;
   
   voidit((int) y);
   voidit((int) z);

   //makePacket(x, (BYTE) magx);
   //makePacket(z, (BYTE) magz);
   //makePacket(y, (BYTE) magy);
   sendData((BYTE*) x, PACKET_SIZE);
   //sendData((BYTE*) z, PACKET_SIZE);
   //sendData((BYTE*) y, PACKET_SIZE);
   // ------------------------
  }
}

/** 
 * sendData(data, length)
 * Params:
 *    - dataToSend: data with 'len' many bytes
 *    - len: number of bytes to send
 */
void sendData(BYTE xdata * dataToSend, BYTE len) {
   int i;
  vDmdTs_RunForTemp( 3 );  // Skip 3 samples.

   vFStep_Apply( (BYTE xdata *) &(arFStepDemo_Data[0]) ); //  Tune to the saved frequency
   //vFCast_FskAdj( 127 );
   vFCast_FskAdj( 12 ); //how far apart the FSK signals are

   //  Wait until there is a demodulated temperature sample
   while ( 0 == bDmdTs_GetSamplesTaken() ) {}
   vPa_Tune( iDmdTs_GetLatestTemp() );
   vStl_PreLoop();
   for (i = 0; i < len; i++) {
      dataToSend[i] = ~dataToSend[i];
   }

   vStl_SingleTxLoop(dataToSend, len);
   voidit((int) dataToSend);
   voidit((int) len);
   vStl_PostLoop();
}

// packet generation function
//    - preamble: 0xFF3D
//    - header:   { length=0 } lenth 0 for debugging purpose 
//    - payload:  newValue
//    - checksum: no checksum for now
void makePacket(BYTE xdata * packet, BYTE * temp, short payload_len) {
   short i;
   packet[0] = 0xFF;     //Preamble 0101010101010101
   packet[1] = 0x3D;     //Preamble 0110010101011011
   for (i = 0; i < payload_len-2; i++) {
      packet[i + 2] = temp[i];
   }
   /*
   packet[2] = newValue; //Device Identifier
   packet[3] = newValue; //Device Identifier
   packet[4] = newValue; //Data
   packet[5] = newValue; //CRC
   packet[6] = newValue; //CRC
   packet[7] = newValue; //CRC
   packet[8] = newValue; //CRC
   packet[9] = newValue; //CRC
   packet[10] = newValue; //CRC
   packet[11] = newValue; //CRC
   */
}

// assert statement to veryfy condition
// if (state == true) { blink LED for numBlinks times } else { turn LED off }
void blink(short state, short numBlinks) {
   short i;
   if (state) {
      for (i = 0; i < numBlinks; i++) {
         GPIO1 ^= 1;
         Delay();
      }
   }
   GPIO1 = 1;
}

void Delay(void) {
    int j;
    int i;
    for(i=0;i<10;i++) {
        for(j=0;j<10000;j++);
    }
}