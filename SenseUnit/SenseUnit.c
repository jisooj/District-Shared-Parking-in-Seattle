#include <stdlib.h>
#include <math.h>

#include "si4010.h"
#include "si4010_api_rom.h"
#include "si4010_api_add.h"
#include "si4010_types.h"
#include "i2c.h"
#include "SenseUnit.h"

//Function Prototypes
void Delay(void);
void randomDelay(void);
void sendData(BYTE xdata * dataToSend, BYTE len);
void invertBits(BYTE xdata * input, BYTE len);
void makePacket(BYTE xdata * dataBuffer);
int averageArray(int* array);
int performDetection();
void system_sleep_si4010();
void init_si4010();
void read_HMC5883L();
int isFirstPower();
// ----------------------------------------------------------

void main(void) {
   BYTE xdata packet[PACKET_SIZE];
   // Initialize si4010 and I2C
   init_si4010();
   I2CInit();

   // Write to slave device
   I2CStart();
   ack = I2CSend(0x3C); //0x3C is magnetometer address
   ack = I2CSend(0x02); //select mode register
   ack = I2CSend(0x00); //continuous measurement
   I2CStop();

   Delay();
   while ( 1 ) {
		found = performDetection();
		
		//Turn off magnetometer (seems to save an additional ~10uA)
		I2CInit();
		I2CStart();
		ack = I2CSend(0x3C); //0x3C is magnetometer address
		ack = I2CSend(0x03); ///select mode register
		ack = I2CSend(0x02); ///continuous measurement
		I2CStop();
		
		//Create and send packets
      makePacket(packet);
      invertBits(packet, PACKET_SIZE);
      sendData((BYTE*) packet, PACKET_SIZE);
      randomDelay();
      sendData((BYTE*) packet, PACKET_SIZE);
      randomDelay();
      sendData((BYTE*) packet, PACKET_SIZE);
      randomDelay();
      sendData((BYTE*) packet, PACKET_SIZE);
      randomDelay();

      system_sleep_si4010();
   }
}

//Set the Si4010 into a low power mode after saving state variables to HVRAM
void system_sleep_si4010() {
   // write to HVRAM. Data written in big endian
   // Memory layout:
   // addr: [0x00,  0x01,  0x02,  0x03, 0x04]
   // data: [MSB_X, LSB_X, MSB_Z, LSB_Z, packetNumber]
   vHvram_Write(packetNumberAddr, packetNumber);
	
   vSleepTim_SetCount(0x01006666); //LWORD(32bit) Value of 24 bit counter (for about 10 seconds) plus a power on bit in position 24 (25th bit)
   vSys_Shutdown();
}

int isFirstPower() {
   return ((SYSGEN & M_POWER_1ST_TIME) >> B_POWER_1ST_TIME) || !GPIO9; //Ground GPIO9 to force initial power
}

void read_HMC5883L() {
	// Tell master to read from address 0x3C
	I2CStart();
	I2CSend(0x3C);
	I2CSend(0x03);
	I2CStop();
	
   I2CStart();
   I2CSend(0x3C | 1);

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
}

void init_si4010() {
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
   rPaSetup.bLevel      = 80; //77 is loud, 20 is quiet, 127 is max
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
   vFCast_Setup();

   vFCast_Tune( 440001227 );
   bFStep_Collect( (BYTE xdata *) &(arFStepDemo_Data[0]) );

   prodID = lSys_GetProdId();
   packetNumber = 0;
	
   srand(prodID); // seed random number generator with si4010 device ID

   // Data written in big endian
   // Memory layout:
   // addr: [0x00,  0x01,  0x02,  0x03, 0x04]
   // data: [MSB_X, LSB_X, MSB_Z, LSB_Z, packetNumber]
   if (!isFirstPower()) {
      packetNumber = bHvram_Read(packetNumberAddr);
   } else {
      vHvram_Write(packetNumberAddr, 0x00);
   }
}

// ------------------------------------------------------

/**
* sendData(data, length)
* Params:
*    - dataToSend: data with 'len' many bytes
*    - len: number of bytes to send
*/
void sendData(BYTE xdata * dataToSend, BYTE len) {
   vDmdTs_RunForTemp( 3 );  // Skip 3 samples.

   vFStep_Apply( (BYTE xdata *) &(arFStepDemo_Data[0]) ); //  Tune to the saved frequency
   vFCast_FskAdj( 12 ); //how far apart the FSK signals are

   //  Wait until there is a demodulated temperature sample
   while ( 0 == bDmdTs_GetSamplesTaken() ) {}
   vPa_Tune( iDmdTs_GetLatestTemp() );
   vStl_PreLoop();
   vStl_SingleTxLoop(dataToSend, len);
   vStl_PostLoop();
}

void invertBits(BYTE xdata * input, BYTE len) {
   int i;
   for (i = 0; i < len; i++) {
      input[i] = ~input[i];
   }
}

// Packet generation function
/*
PACKET DEFINITION
- preamble: 2 bytes for 0xFF3D
- header: 4 bytes for ID
- payload: {
1 byte { car status (parked or not) 1bit, packet number 7bits}
1 byte temperature in celcius,
2 bytes battery voltage unit (mV)
}
- checksum: no checksum for now
*/
void makePacket(BYTE xdata * packet) {
   unsigned int battery_mV;
   battery_mV = (unsigned int) iMVdd_Measure(0);

   // Preamble
   packet[0] = 0xFF;     //Preamble 0101010101010101
   packet[1] = 0x3D;     //Preamble 0110010101011011

   // Header: 4 byte ID which is unsigned long int
   packet[2] = (BYTE) ((prodID >> 24) & 0xFF);
   packet[3] = (BYTE) ((prodID >> 16) & 0xFF);
   packet[4] = (BYTE) ((prodID >> 8) & 0xFF);
   packet[5] = (BYTE) (prodID & 0xFF);

   // Payload: car status, packet number, temp, battery voltage
   packet[6] = ((found << 7) | packetNumber);
   vDmdTs_RunForTemp( 3 );  // Skip 3 samples.
   while ( 0 == bDmdTs_GetSamplesTaken() ) {}
   packet[7] = iDmdTs_GetLatestTemp() / 220 + 25;
   packet[8] = (BYTE) ((battery_mV & 0xFF00) >> 8);
   packet[9] = (BYTE) (battery_mV & 0xFF);

   // packeNumber range: [0, 127]
   packetNumber = (packetNumber + 1) % 0x7F;
}

void Delay(void) {
   int j;
   int i;
   for(i=0;i<10;i++) {
      for(j=0;j<10000;j++);
   }
}

void randomDelay(void) {
   // wiIntervalCount is of type WORD.
   // MAX_INTERVAL = 2^16 = 65536
   // This delay cycle ranges [63536, 65536)
   vSys_16BitDecLoop(63536 + rand() % 2000);
}

// ------------- CAT DETECTION ALGORITHM ----------------
//returns the result of if a car is in the parking spot or not
int performDetection() {
	short i;
	int difference_X;
	int difference_Z;
	int signalAverage_X;
	int signalAverage_Z;

	//get 20 samples
	for (i = 0; i < BUFFER_SIZE; i++) { 
		read_HMC5883L(); //get fresh magnetometer data
		sampleSize_X[i] = magx;  // add new value to array
		sampleSize_Z[i] = magz;
		Delay();
	}
	
	//average samples
	average_X = averageArray(sampleSize_X);
	average_Z = averageArray(sampleSize_Z);
	
	//save average at power up to compare against later
	if (isFirstPower()) {
		signalAverage_X = average_X;
		signalAverage_Z = average_Z;
		vHvram_Write(calibAddrX,  ((signalAverage_X >> 8) & 0xFF));
		vHvram_Write(calibAddrX + 1, (signalAverage_X & 0xFF));
		vHvram_Write(calibAddrZ, ((signalAverage_Z >> 8) & 0xFF));
		vHvram_Write(calibAddrZ + 1, (signalAverage_Z & 0xFF));
		return 0; //assuming no car at power up
	} 
	signalAverage_X = (int) bHvram_Read(calibAddrX) << 8;
	signalAverage_X |= bHvram_Read(calibAddrX + 1);
	signalAverage_Z = (int) bHvram_Read(calibAddrZ) << 8;
	signalAverage_Z |= bHvram_Read(calibAddrZ + 1);

	//compare magnetometer values to find car
	difference_X = abs(average_X - signalAverage_X); // x axis or y
	difference_Z = abs(average_Z - signalAverage_Z); // z axis
	
	return (difference_X >= THRESHOLD || difference_Z >= THRESHOLD);
}

// Returns an average of array
int averageArray(int* ar) {
   int i;
   int average = 0;
   for(i = 0; i < BUFFER_SIZE; i++) {
      average += ar[i];
   }
   return (average / BUFFER_SIZE);
}
