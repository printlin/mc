#ifndef __OLEDL_H
#define __OLEDL_H

#include "myloop.h"

#define SPI1_MOSI_PIN		7
#define SPI1_MOSI_PORT		GPIOA

#define SPI1_SCK_PIN		5
#define SPI1_SCK_PORT		GPIOA

#define SPI1_NSS_PIN		4
#define SPI1_NSS_PORT		GPIOA

#define SSD1327_RS_PIN		6
#define SSD1327_RS_PORT		GPIOA

#define OLED_CS(a)		\
						if (a)											\
						GPIOOutputHig(SPI1_NSS_PORT,SPI1_NSS_PIN);		\
						else											\
						GPIOOutputLow(SPI1_NSS_PORT,SPI1_NSS_PIN);
#define OLED_CLK(a)		\
						if (a)											\
						GPIOOutputHig(SPI1_SCK_PORT,SPI1_SCK_PIN);		\
						else											\
						GPIOOutputLow(SPI1_SCK_PORT,SPI1_SCK_PIN);
#define OLED_SDA(a)		\
						if (a)											\
						GPIOOutputHig(SPI1_MOSI_PORT,SPI1_MOSI_PIN);	\
						else											\
						GPIOOutputLow(SPI1_MOSI_PORT,SPI1_MOSI_PIN);
#define OLED_RS(a)		\
						if (a)											\
						GPIOOutputHig(SSD1327_RS_PORT,SSD1327_RS_PIN);	\
						else											\
						GPIOOutputLow(SSD1327_RS_PORT,SSD1327_RS_PIN);

#define OLED_X_HALF			0x3f//128
#define OLED_Y				0x5f//96

/*
 * OLED坐标说明：每个单位的X坐标包括两个像素，其值分别由指令0xff的高低位决定。
 * 
 *   X-----------------------------128/2------------------------------->
 * Y	       (0xf8)
 * |	    0 - 1 - 2 - 3 - 4 - 5 - 6  ******   59 -  60 -  61 -  62 -  63
 * |	    |   |   |   |   |   |   |           |     |     |     |     |
 * |	 0- + +(f 8)+ + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |	 1- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |	 2- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |	 3- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |	 4- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |	 5- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |	 6- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |	 7- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |
 * |	 *                             ******
 * |	 *                             ******
 * |	 *                             ******
 * |
 * |	92- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |	93- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * |	94- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * V	95- + + + + + + + + + + + + +  ******   +  +  +  +  +  +  +  +  +
 * 
 */

void OLEDPrintChar1608(uint8_t x,uint8_t y,uint8_t val,char num);
void OLEDPrintString1608(uint8_t x,uint8_t y,uint8_t val,char *str);

#ifdef GRAM_USED
void OLEDPrintDot(uint8_t x,uint8_t y,uint8_t val);
#endif
void OLEDPrintDualDot(uint8_t x,uint8_t y,uint8_t val);
void OLEDPrintQuadDot(uint8_t x,uint8_t y,uint8_t val);
void OLEDClear(uint8_t val);

void SSD1327Init(void);

#endif
