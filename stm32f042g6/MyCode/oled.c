#include "oled.h"

#include "ASCII.h"

static void LCD_WriteByteSPI(uint8_t byte)
{
	uint8_t buf;
	
    for(uint8_t i=0;i<8;i++)
    {
        buf = (byte >> (7-i)) &0x01;
        OLED_SDA(buf);
		OLED_CLK(0);
        OLED_CLK(1);
    }
}

static void LCD_WriteOneSPI(uint8_t one)
{
	uint8_t buf;
	
    for(uint8_t i=0;i<4;i++) 
    {
        buf = (one >> (3-i)) &0x01;
        OLED_SDA(buf);
		OLED_CLK(0);
        OLED_CLK(1);
    }
}

static void WriteCmd(uint8_t cmd)
{
	OLED_CS(0);
	OLED_RS(0);
	LCD_WriteByteSPI(cmd);//upper eight bits
	OLED_RS(1);
	OLED_CS(1);
}

static void WriteDat(uint8_t dat)
{
	OLED_CS(0);
	OLED_RS(1);
	LCD_WriteByteSPI(dat);//upper eight bits
	OLED_RS(0);
	OLED_CS(1);
}

/*
 * OLED刷新：
 * 先根据两点对角的坐标，选取一块矩形区域，然后从该区域的第一行第一列开始刷新。
 * 每个数据为8位，每次刷新只能写两个像素，每个像素4位灰度。
 * 每两个像素为一个横坐标单位，如一行有128个像素，需要刷新64次。
 * 数据高4位为该坐标的值，低四位为第二个像素的值。
 */
static void OLEDAreaSet(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2)
{
	WriteCmd(0x15);//SET COLUMN ADDR
	WriteCmd(x1);//横坐标只能一次写两个像素
	WriteCmd(x2);
	WriteCmd(0x75);//SET ROW ADDR
	WriteCmd(y1);
	WriteCmd(y2);
}

void OLEDPrintChar1608(uint8_t x,uint8_t y,uint8_t val,char num)
{       
	uint8_t temp;
	uint16_t i;   
	
	if(val>0xf){val&=0xf;}//输入超过0xf，清除高4位
	
	WriteCmd(0x15);//SET COLUMN ADDR 
	WriteCmd(x); 
	WriteCmd(x+3); 
	WriteCmd(0x75);//SET ROW ADDR 
	WriteCmd(y); 
	WriteCmd(y+15); 
	 
	num -= ' ';//得到偏移后的值
	i = num * 16;
	
	OLED_RS(1);
	OLED_CS(0); 

	for(uint8_t pos=0;pos<16;pos++)
	{
		temp=nAsciiDot[i+pos];//ASCII字库
		for(uint8_t t=0;t<8;t++)
		{                 
			if(temp&0x80)
			{
				LCD_WriteOneSPI(val);
			}
			else 
			{
				LCD_WriteOneSPI(0x00);
			}
			temp<<=1; 
		}
	}
	OLED_CS(1);
	OLED_RS(1);
}

void OLEDPrintString1608(uint8_t x,uint8_t y,uint8_t val,char *str)
{
	uint8_t len=0;
	
	while(*str)
	{
		OLEDPrintChar1608(x+len,y,val,*str);
		str++;
		len+=4;
	}
}

#ifdef GRAM_USED
/*
 * 由于每次刷新只能写两个像素，为防止影响到另外一个点，需要写入单个像素前需读取另一个像素的值，再一起写入
 * 所以建立二位数组[128*64]作为OLED点阵，把每个像素的值都写入到数组里作为缓存
 * 128*96的16阶OLED需要12.288kbytes的SRAM，但stm32f042g6只有6k，所以暂时不支持单点写入的方法
 */
uint8_t OLEDGRam[OLED_X][OLED_Y];

void OLEDPrintDot(uint8_t x,uint8_t y,uint8_t val)
{
	OLEDAreaSet(x,y,x,y);

	OLEDGRam[x][y] = val;
	//读取右边一个像素的值并写入
	WriteDat(val<<4 | OLEDDisplayCache[x+1][y]);
}
#endif

/*
 * 画两个点，x为开始坐标
 * val高4bit为左边的点，低4bit为右边的点
 */
void OLEDPrintDualDot(uint8_t x,uint8_t y,uint8_t val)
{
	OLEDAreaSet(x,y,x,y);
	WriteDat(val);
}

/*
 * 画四个点，x为开始坐标
 * val高4bit为左边的像素，低4bit为右边的像素
 */
void OLEDPrintQuadDot(uint8_t x,uint8_t y,uint8_t val)
{
	OLEDAreaSet(x,y,x,y+1);
	WriteDat(val);
	WriteDat(val);
}

/*
 * 清屏
 */
void OLEDClear(uint8_t val)
{
	if(val>0xf){val&=0xf;}//输入超过0xf，清除高4位
	
	OLEDAreaSet(0,0,OLED_X_HALF,OLED_Y);
	
	for(uint8_t y=0;y<=OLED_Y;y++)
	{
		for(uint8_t x=0;x<=OLED_X_HALF;x++)
		{
			WriteDat(val<<4 | val);
		}
	}
}

static void Spi1Init(void)
{
	GPIOInit(SPI1_MOSI_PORT,SPI1_MOSI_PIN,MODER_OUT,OTYPER_PP,OSPEEDR_HIG,PUPDR_NONE);
	GPIOInit(SPI1_SCK_PORT,SPI1_SCK_PIN,MODER_OUT,OTYPER_PP,OSPEEDR_HIG,PUPDR_NONE);
	GPIOInit(SPI1_NSS_PORT,SPI1_NSS_PIN,MODER_OUT,OTYPER_PP,OSPEEDR_HIG,PUPDR_NONE);
	GPIOInit(SSD1327_RS_PORT,SSD1327_RS_PIN,MODER_OUT,OTYPER_PP,OSPEEDR_HIG,PUPDR_NONE);
	
	OLED_CS(0);
	OLED_RS(0);
	OLED_CLK(0);
	OLED_SDA(0);
}

void SSD1327Init(void)
{
	Spi1Init();
	
	WriteCmd(0xae);
	WriteCmd(0x15);//设置列地址
	WriteCmd(0x00);//start column	0
	WriteCmd(OLED_Y);//end column
	WriteCmd(0x75);//设置行地址
	WriteCmd(0x00);//start row		0
	WriteCmd(OLED_X_HALF);//end row
	WriteCmd(0x81);//设置对比度
	WriteCmd(0x80);
	WriteCmd(0xa0);//segment remap
	WriteCmd(0x51);
	WriteCmd(0xa1);//start line
	WriteCmd(0x00);
	WriteCmd(0xa2);//display offset
	WriteCmd(0x00);
	WriteCmd(0xa4);//normal display
	WriteCmd(0xa8);//set multiplex ratio
	WriteCmd(0x7f);
	WriteCmd(0xb1);//set phase leghth
	WriteCmd(0xf1);
	WriteCmd(0xb3);//set dclk
	WriteCmd(0x00);//80Hz:0xc1	90Hz:0xe1  100Hz:0x00	110Hz:0x30 120Hz:0x50 130Hz:0x70
	WriteCmd(0xab);
	WriteCmd(0x01);
	WriteCmd(0xb6);//set phase leghth
	WriteCmd(0x0f);
	WriteCmd(0xbe);
	WriteCmd(0x0f);
	WriteCmd(0xbc);
	WriteCmd(0x08);
	WriteCmd(0xd5);
	WriteCmd(0x62);
	WriteCmd(0xfd);
	WriteCmd(0x12);
	WriteCmd(0xaf); 
	
#ifdef GRAM_USED
	for(uint8_t y=0;y<=OLED_Y;y++)
	{
		for(uint8_t x=0;x<=OLED_X;x++)
		{
			OLEDDisplayCache[x][y] = 0x00;
		}
	}
#endif

	OLEDClear(0x00);
}
