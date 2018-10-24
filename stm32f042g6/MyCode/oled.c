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
 * OLEDˢ�£�
 * �ȸ�������Խǵ����꣬ѡȡһ���������Ȼ��Ӹ�����ĵ�һ�е�һ�п�ʼˢ�¡�
 * ÿ������Ϊ8λ��ÿ��ˢ��ֻ��д�������أ�ÿ������4λ�Ҷȡ�
 * ÿ��������Ϊһ�������굥λ����һ����128�����أ���Ҫˢ��64�Ρ�
 * ���ݸ�4λΪ�������ֵ������λΪ�ڶ������ص�ֵ��
 */
static void OLEDAreaSet(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2)
{
	WriteCmd(0x15);//SET COLUMN ADDR
	WriteCmd(x1);//������ֻ��һ��д��������
	WriteCmd(x2);
	WriteCmd(0x75);//SET ROW ADDR
	WriteCmd(y1);
	WriteCmd(y2);
}

void OLEDPrintChar1608(uint8_t x,uint8_t y,uint8_t val,char num)
{       
	uint8_t temp;
	uint16_t i;   
	
	if(val>0xf){val&=0xf;}//���볬��0xf�������4λ
	
	WriteCmd(0x15);//SET COLUMN ADDR 
	WriteCmd(x); 
	WriteCmd(x+3); 
	WriteCmd(0x75);//SET ROW ADDR 
	WriteCmd(y); 
	WriteCmd(y+15); 
	 
	num -= ' ';//�õ�ƫ�ƺ��ֵ
	i = num * 16;
	
	OLED_RS(1);
	OLED_CS(0); 

	for(uint8_t pos=0;pos<16;pos++)
	{
		temp=nAsciiDot[i+pos];//ASCII�ֿ�
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
 * ����ÿ��ˢ��ֻ��д�������أ�Ϊ��ֹӰ�쵽����һ���㣬��Ҫд�뵥������ǰ���ȡ��һ�����ص�ֵ����һ��д��
 * ���Խ�����λ����[128*64]��ΪOLED���󣬰�ÿ�����ص�ֵ��д�뵽��������Ϊ����
 * 128*96��16��OLED��Ҫ12.288kbytes��SRAM����stm32f042g6ֻ��6k��������ʱ��֧�ֵ���д��ķ���
 */
uint8_t OLEDGRam[OLED_X][OLED_Y];

void OLEDPrintDot(uint8_t x,uint8_t y,uint8_t val)
{
	OLEDAreaSet(x,y,x,y);

	OLEDGRam[x][y] = val;
	//��ȡ�ұ�һ�����ص�ֵ��д��
	WriteDat(val<<4 | OLEDDisplayCache[x+1][y]);
}
#endif

/*
 * �������㣬xΪ��ʼ����
 * val��4bitΪ��ߵĵ㣬��4bitΪ�ұߵĵ�
 */
void OLEDPrintDualDot(uint8_t x,uint8_t y,uint8_t val)
{
	OLEDAreaSet(x,y,x,y);
	WriteDat(val);
}

/*
 * ���ĸ��㣬xΪ��ʼ����
 * val��4bitΪ��ߵ����أ���4bitΪ�ұߵ�����
 */
void OLEDPrintQuadDot(uint8_t x,uint8_t y,uint8_t val)
{
	OLEDAreaSet(x,y,x,y+1);
	WriteDat(val);
	WriteDat(val);
}

/*
 * ����
 */
void OLEDClear(uint8_t val)
{
	if(val>0xf){val&=0xf;}//���볬��0xf�������4λ
	
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
	WriteCmd(0x15);//�����е�ַ
	WriteCmd(0x00);//start column	0
	WriteCmd(OLED_Y);//end column
	WriteCmd(0x75);//�����е�ַ
	WriteCmd(0x00);//start row		0
	WriteCmd(OLED_X_HALF);//end row
	WriteCmd(0x81);//���öԱȶ�
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
