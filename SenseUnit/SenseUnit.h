#ifndef _SENSE_UNIT_H
#define _SENSE_UNIT_H
#include "si4010_types.h"

#define bFStepDemo_MaxFrameSize_c (125)
#define bFStepDemo_FreqNum_c (1)
#define INT_MIN -32767
#define INT_MAX 32767
#define PACKET_SIZE 10   // including preamble, heading, payload
#define THRESHOLD 50
#define BUFFER_SIZE 20

//Types
typedef BYTE tFStepDemo_Data[bFStep_DataLength_c];
tFStepDemo_Data xdata arFStepDemo_Data[bFStepDemo_FreqNum_c];

tOds_Setup xdata rOdsSetup;
tPa_Setup xdata  rPaSetup;

unsigned char ack;

// Data order: magx magz magy
short xdata magx;
short xdata magz;
short xdata magy;

LWORD xdata prodID;
BYTE xdata packetNumber; 
// --------- Variable used in Car Detection ---------
// variables in order to implement moving average
int xdata average_X = 0;
int xdata average_Z = 0;

int xdata counter = 1;
int xdata index = 0;

int xdata sampleSize_X[BUFFER_SIZE];  // store 20 samples of mag data
int xdata sampleSize_Z[BUFFER_SIZE];
//int xdata signalAverage_X;   // average signal based on the initial data
//int xdata signalAverage_Z;   // average signal based on the initial data
//int xdata threshold = 0;

BYTE xdata found = 0;
//int xdata difference_X = 0;
//int xdata difference_Z = 0;

// Calibration data used when Si4010 wakes up from low power mode
// Only bottom 3 bits of the address will be used.
BYTE xdata calibAddrX = 0x00; // addr 0x00, 0x01 stores int value for X-axis
BYTE xdata calibAddrZ = 0x02; // addr 0x02, 0x03 stores int value for Z-axis
BYTE xdata packetNumberAddr = 0x04; // addr 0x04 stores BYTE value for packetNumber


/**
PACKET DEFINITION
- preamble: 2 bytes for 0xFF3D
- header: 4 bytes for ID
- payload: { 
  1 byte { car status (parked or not) 1bit, packet number 7bits}
  1 byte temperature (Celcius),
  2 bytes battery voltage unit (mV)
} 
- checksum: no checksum for now

TOTAL PACKET LENGTH = 10

--------------------------------------------------------------------------
Type                Bit-Width   Type-Definition   Prefix
unsigned char       8           BYTE              b
unsigned int        16          WORD              w
unsigned long int   32          LWORD             l
signed char         8           CHAR              c
int                 16                            i
long int            32                            j
float               32                            f

--------------------------------------------------------------------------
 */

#endif /* _SENSE_UNIT_H */
