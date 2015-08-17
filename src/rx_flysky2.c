#if 0
/*
 * rx_flysky.c
 *
 *  Created on: 11 juil. 2015
 *      Author: franck
 */


#include "bradwii.h"
#include "rx.h"
#include "lib_soft_3_wire_spi.h"
#include "a7105.h"
#include "lib_timers.h"
#include <stdint.h>
#include "config_X4.h"


//#define 	INT32_MAX   0x7fffffffL

//#define 	INT32_MIN   (-INT32_MAX - 1L)
//#define 	INT16_MAX   64638
//#define 	INT16_MIN   -65664

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

#define A7105_SCS   (DIGITALPORT1 | 4)
#define A7105_SCK   (DIGITALPORT1 | 3)
#define A7105_SDIO  (DIGITALPORT1 | 2)

static uint8_t aid[4];

static uint32_t id;


static const uint8_t tx_channels[16][16] = { { 0x0a, 0x5a, 0x14, 0x64, 0x1e,
		0x6e, 0x28, 0x78, 0x32, 0x82, 0x3c, 0x8c, 0x46, 0x96, 0x50, 0xa0 }, {
		0xa0, 0x50, 0x96, 0x46, 0x8c, 0x3c, 0x82, 0x32, 0x78, 0x28, 0x6e, 0x1e,
		0x64, 0x14, 0x5a, 0x0a }, { 0x0a, 0x5a, 0x50, 0xa0, 0x14, 0x64, 0x46,
		0x96, 0x1e, 0x6e, 0x3c, 0x8c, 0x28, 0x78, 0x32, 0x82 }, { 0x82, 0x32,
		0x78, 0x28, 0x8c, 0x3c, 0x6e, 0x1e, 0x96, 0x46, 0x64, 0x14, 0xa0, 0x50,
		0x5a, 0x0a }, { 0x28, 0x78, 0x0a, 0x5a, 0x50, 0xa0, 0x14, 0x64, 0x1e,
		0x6e, 0x3c, 0x8c, 0x32, 0x82, 0x46, 0x96 },
		{ 0x96, 0x46, 0x82, 0x32, 0x8c, 0x3c, 0x6e, 0x1e, 0x64, 0x14, 0xa0,
				0x50, 0x5a, 0x0a, 0x78, 0x28 },
		{ 0x50, 0xa0, 0x28, 0x78, 0x0a, 0x5a, 0x1e, 0x6e, 0x3c, 0x8c, 0x32,
				0x82, 0x46, 0x96, 0x14, 0x64 },
		{ 0x64, 0x14, 0x96, 0x46, 0x82, 0x32, 0x8c, 0x3c, 0x6e, 0x1e, 0x5a,
				0x0a, 0x78, 0x28, 0xa0, 0x50 },
		{ 0x50, 0xa0, 0x46, 0x96, 0x3c, 0x8c, 0x28, 0x78, 0x0a, 0x5a, 0x32,
				0x82, 0x1e, 0x6e, 0x14, 0x64 },
		{ 0x64, 0x14, 0x6e, 0x1e, 0x82, 0x32, 0x5a, 0x0a, 0x78, 0x28, 0x8c,
				0x3c, 0x96, 0x46, 0xa0, 0x50 },
		{ 0x46, 0x96, 0x3c, 0x8c, 0x50, 0xa0, 0x28, 0x78, 0x0a, 0x5a, 0x1e,
				0x6e, 0x32, 0x82, 0x14, 0x64 },
		{ 0x64, 0x14, 0x82, 0x32, 0x6e, 0x1e, 0x5a, 0x0a, 0x78, 0x28, 0xa0,
				0x50, 0x8c, 0x3c, 0x96, 0x46 },
		{ 0x46, 0x96, 0x0a, 0x5a, 0x3c, 0x8c, 0x14, 0x64, 0x50, 0xa0, 0x28,
				0x78, 0x1e, 0x6e, 0x32, 0x82 },
		{ 0x82, 0x32, 0x6e, 0x1e, 0x78, 0x28, 0xa0, 0x50, 0x64, 0x14, 0x8c,
				0x3c, 0x5a, 0x0a, 0x96, 0x46 },
		{ 0x46, 0x96, 0x0a, 0x5a, 0x50, 0xa0, 0x3c, 0x8c, 0x28, 0x78, 0x1e,
				0x6e, 0x32, 0x82, 0x14, 0x64 },
		{ 0x64, 0x14, 0x82, 0x32, 0x6e, 0x1e, 0x78, 0x28, 0x8c, 0x3c, 0xa0,
				0x50, 0x5a, 0x0a, 0x96, 0x46 }, };
static const uint8_t A7105_regs[] = { 0xff, 0x42, 0x00, 0x14, 0x00, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x21, 0x05, 0x00, 0x50, 0x9e, 0x4b, 0x00,
		0x02, 0x16, 0x2b, 0x12, 0x00, 0x62, 0x80, 0x80, 0x00, 0x0a, 0x32, 0xc3,
		0x0f, 0x13, 0xc3, 0x00, 0xff, 0x00, 0x00, 0x3b, 0x00, 0x17, 0x47, 0x80,
		0x03, 0x01, 0x45, 0x18, 0x00, 0x01, 0x0f, 0xff, };

static uint8_t packet[21];
//########## Variables #################
static uint8_t chanrow;
static uint8_t chancol;
static uint8_t chanoffset;
static uint8_t channel;




extern globalstruct global;



void Read_Packet(void);
void init_channels(void);
int _readrx(void);
//BIND_TX
uint32_t bind_Flysky() {
		//
	//	word counter1=512;
	//	int use_settings=0;//utilisation de l'eeprom pour charger l'id si non detect?
	//	uint8_t binded=0;
	//	uint8_t wait_for_bind=1;
	//	while(wait_for_bind){
	//		A7105_Strobe(0xA0);
	//		A7105_Strobe(0xF0);
	//		A7105_WriteRegister(0x0F,0x00);//binding listen on channel 0
	//		A7105_Strobe(0xC0);
	//		delay(10);//wait 10ms
	//		if (bitRead(counter1,2)==1){
	//			Red_LED_ON;
	//		}
	//		if(bitRead(counter1,2)==0){
	//			Red_LED_OFF;
	//		}
	//
	//		if (GIO_0){
	//			uint8_t x=A7105_ReadRegister(A7105_00_MODE);
	//			if ((bitRead(x,5)==0)&&(bitRead(x,6)==0)){//test CRC&CRF bits
	//				Read_Packet();
	//				uint8_t i;
	//				uint8_t adr=10;
	//				for(i=0;i<4;i++) {
	//#ifdef USE_EEPROM
	//					EEPROM.write(adr,packet[i+1]);
	//#endif
	//					adr=adr+1;
	//					txid[i]=packet[i+1];
	//				}
	//				binded=1;
	//				wait_for_bind=0;
	//
	//			}
	//		}
	//
	//		counter1--;
	//		if (!binded && use_settings) {
	//			wait_for_bind=counter1>0;
	//		}
	//	}
	//	if (!binded) {
	//		uint8_t i;
	//		uint8_t adr=10;
	//		for(i=0;i<4;i++){
	//			txid[i]=EEPROM.read(adr);
	//			adr=adr+1;
	//		}
	//	}
	//#ifdef DEBUG
	//	Serial.println("binded");
	//#endif
	//
	//
	//	uint32_t _id=(txid[0] | ((uint32_t)txid[1]<<8) | ((uint32_t)txid[2]<<16) | ((uint32_t)txid[3]<<24));
	//	return _id;
	global.failsafetimer = lib_timers_starttimer();
	return 0x1749;
}

uint32_t convert2id(uint8_t txid0,uint8_t txid1,uint8_t txid2,uint8_t txid3) {
	return txid0 | ((uint32_t)txid1<<8) | ((uint32_t)txid2<<16) | ((uint32_t)txid3<<24);
}

void Read_Packet() {
	A7105_ReadPayload((uint8_t*)&packet,21);
}

void init_a7105(void) {
	uint8_t i;
	uint8_t if_calibration1;
	uint8_t vco_calibration0;
	uint8_t vco_calibration1;
	lib_timers_delaymilliseconds(10);	//wait 10ms for A7105 wakeup
		A7105_Reset();	//reset A7105
		A7105_WriteID(0x5475c52A);	//A7105 id
		A7105_ReadID((uint8_t*)&aid);
//		Serial.print(aid[0], HEX);
//		Serial.print(aid[1], HEX);
//		Serial.print(aid[2], HEX);
//		Serial.print(aid[3], HEX);
//		Serial.println("fin id");
		for (i = 0; i < 0x33; i++) {
			if (A7105_regs[i] != 0xff)
				A7105_WriteRegister(i, A7105_regs[i]);

		}
		A7105_Strobe(A7105_STANDBY);	//stand-by
		A7105_WriteRegister(0x02, 0x01);
		while (A7105_ReadRegister(0x02)) {
			if_calibration1 = A7105_ReadRegister(0x22);
			if (if_calibration1 & 0x10) {	//do nothing
			}
		}

		A7105_WriteRegister(0x24, 0x13);
		A7105_WriteRegister(0x26, 0x3b);
		A7105_WriteRegister(0x0F, 0x00);	//channel 0
		A7105_WriteRegister(0x02, 0x02);
		while (A7105_ReadRegister(0x02)) {
			vco_calibration0 = A7105_ReadRegister(0x25);
			if (vco_calibration0 & 0x08) {	//do nothing
			}
		}

		A7105_WriteRegister(0x0F, 0xA0);
		A7105_WriteRegister(0x02, 0x02);
		while (A7105_ReadRegister(0x02)) {
			vco_calibration1 = A7105_ReadRegister(0x25);
			if (vco_calibration1 & 0x08) {	//do nothing
			}
		}

		A7105_WriteRegister(0x25, 0x08);
		A7105_Strobe(A7105_STANDBY);	//stand-by
}

void initrx(void) {

	lib_soft_3_wire_spi_init(A7105_SDIO, A7105_SCK, A7105_SCS);
	init_a7105();
	id=bind_Flysky();

	init_channels();
	
	while (!_readrx()) {
		 if( lib_timers_gettimermicroseconds(0) % 500000 > 250000)
//			  x4_set_leds(X4_LED_FR | X4_LED_RL);
     else
 //       x4_set_leds(X4_LED_FL | X4_LED_RR);
	}
}


void init_channels() {
	chanrow = id % 16;
	chanoffset = (id & 0xff) / 16;
	chancol = 0;
//	Serial.print("chanoffset=");
//	Serial.print(chanoffset, HEX);
	if (chanoffset > 9)
		chanoffset = 9;	//from sloped soarer findings, bug in flysky protocol
	//initiallize default ppm values
//	for (int i = 0; i < chanel_number; i++) {
//		ppm[i] = default_servo_value;
//	}
}

fixedpointnum scaleValue(uint16_t value, uint16_t minInput, uint16_t maxInput, fixedpointnum minOutput, fixedpointnum maxOutput) {
	fixedpointnum temp=value;
	if (value<minInput) temp=minInput;
	if (value>maxInput) temp=maxInput;
	fixedpointnum scalefactor=(maxOutput-minOutput)/(maxInput-minInput);
	temp-=(maxInput+minInput)/2;
	return temp*scalefactor;
}

void rx_fp_lowpassfilter(fixedpointnum *variable, fixedpointnum newvalue, fixedpointnum timesliver, fixedpointnum oneoverperiod, int timesliverextrashift) {
	//lib_fp_lowpassfilter(variable, newvalue, timesliver, oneoverperiod, timesliverextrashift);
	*variable=newvalue;
}

void decodepacket() {

	rx_fp_lowpassfilter(&global.rxvalues[YAWINDEX],
			scaleValue(packet[5] + 256 * packet[6], 1000,2000,-32319,32319)
			/*((fixedpointnum) word_temp )*/, global.timesliver, 0L, 0);
	rx_fp_lowpassfilter(&global.rxvalues[ROLLINDEX],
			scaleValue(packet[7] + 256 * packet[8], 1000,2000,-32319,32319)
			/*((fixedpointnum) word_temp )*/, global.timesliver, 0L, 0);
	rx_fp_lowpassfilter(&global.rxvalues[THROTTLEINDEX],
			scaleValue(packet[9] + 256 * packet[10], 1000,2000,-65664,64638)
			/*((fixedpointnum) word_temp )*/, global.timesliver, 0L, 0);
	rx_fp_lowpassfilter(&global.rxvalues[PITCHINDEX],
			scaleValue(packet[11] + 256 * packet[12], 1000,2000,-32319,32319)
			/*((fixedpointnum) word_temp )*/, global.timesliver, 0L, 0);
	rx_fp_lowpassfilter(&global.rxvalues[AUX1INDEX],
			scaleValue(packet[13] + 256 * packet[14], 1000,2000,-65664,65664)
			/*((fixedpointnum) word_temp )*/, global.timesliver, 0L, 0);
	rx_fp_lowpassfilter(&global.rxvalues[AUX2INDEX],
			scaleValue(packet[15] + 256 * packet[16], 1000,2000,-65664,64638)
			/*((fixedpointnum) word_temp )*/, global.timesliver, 0L, 0);
	rx_fp_lowpassfilter(&global.rxvalues[AUX3INDEX],
			scaleValue(packet[17] + 256 * packet[18], 1000,2000,-65664,64638)
			/*((fixedpointnum) word_temp )*/, global.timesliver, 0L, 0);
	rx_fp_lowpassfilter(&global.rxvalues[AUX4INDEX],
			scaleValue(packet[19] + 256 * packet[20], 1000,2000,-65664,64638)
			/*((fixedpointnum) word_temp )*/, global.timesliver, 0L, 0);
}

int _readrx(void) {
	for (int i=0;i<21;i++) packet[i]=0;
	channel=tx_channels[chanrow][chancol]-1-chanoffset;
	A7105_Strobe(A7105_STANDBY);
	A7105_Strobe(A7105_RST_RDPTR);
	A7105_WriteRegister(0x0F, channel);
	A7105_Strobe(A7105_RX);
	chancol = (chancol + 1) % 16;
	unsigned long pause;
	uint8_t x;
	pause=lib_timers_starttimer();
	while(1){
		if (lib_timers_gettimermicroseconds(pause) > 2000) {
//			Red_LED_OFF;
			chancol = (chancol + 1) % 16;
			channel=tx_channels[chanrow][chancol]-1-chanoffset;
			break;
		}
		if(A7105_ReadRegister(A7105_00_MODE) & A7105_MODE_TRER_MASK){
			continue;
		}
		x=A7105_ReadRegister(0x00);
		if (!(bitRead(x,5)==0)&& !(bitRead(x,6)==0)){
			continue;
		}
		Read_Packet();
		if (convert2id(packet[1],packet[2],packet[3],packet[4])!=id) {
//			Serial.println("bad id");
			continue;
		}


		decodepacket();

		// reset the failsafe timer
		global.failsafetimer = lib_timers_starttimer();
		return 1;
	}
	return 0;
}


void readrx(void) {
	_readrx();
}
#endif

