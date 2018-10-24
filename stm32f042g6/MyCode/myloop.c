#include "myloop.h"
#include <math.h>
#include "cmsis_os2.h"
#include <stdio.h>



#define USART2_TX_PIN			2
#define USART2_TX_PORT			GPIOA

#define USART2_RX_PIN			3
#define USART2_RX_PORT			GPIOA

uint8_t Usart2Buf[0xFF][0x08];//数据缓存
uint8_t Usart2BufLen[0xff];//数据帧长度
uint8_t Usart2BufMun;
uint8_t Usart2BufFlag;


char Str_buff[20];
uint8_t DotBuff_sin[64];
uint8_t ShowBuff_sin[64];
uint8_t DotBuff_cos[64];
uint8_t ShowBuff_cos[64];
uint8_t DotBuff_sinPi[64];
uint8_t ShowBuff_sinPi[64];
uint8_t LineLen=64;
uint8_t LineHei=96;

int fputc(int ch, FILE *f)
{
	while(!(USART2->ISR & USART_ISR_TXE));//等待发送区空
	USART2->TDR = (uint8_t)(ch);//循环发送,直到发送完毕
	return ch;
}




void USART2Init(uint32_t buad);
void showSin();
void showCos();
void showSinPi();
void initData();

void setup(void)
{
	BspInit();
	SSD1327Init();
	
	GPIOInit(GPIOB,4,MODER_IN,0,0,PUPDR_PU);
	initData();
	
	printf("setup");
	osKernelInitialize();
	osThreadNew(showSin,NULL,NULL);
	osThreadNew(showCos,NULL,NULL);
	osThreadNew(showSinPi,NULL,NULL);
	osKernelStart();
}

void loop(void)
{
//	showSin();
//		showCos();
//		showSinPi();
}


void initData()
{
	for(uint8_t i=0;i<LineLen;i++)
	{
		uint8_t y_sin=(uint8_t)(sin(((2*PI)/LineLen)*i)*LineHei/2 + LineHei/2);
		DotBuff_sin[i]=y_sin;
		uint8_t y_cos=(uint8_t)(cos(((2*PI)/LineLen)*i)*LineHei/2 + LineHei/2);
		DotBuff_cos[i]=y_cos;
		uint8_t y_pi=(uint8_t)(sin(((2*PI)/LineLen)*i+PI)*LineHei/2 + LineHei/2);
		DotBuff_sinPi[i]=y_pi;
	}
}
void showSin()
{
	uint8_t index;
	for(uint8_t LineIndex=0;LineIndex<LineLen;LineIndex++){
		for(uint8_t i=LineLen;i>0;)
		{
			i--;
			index=(i+LineIndex+1)%LineLen;
			uint8_t y=DotBuff_sin[index];
			OLEDPrintDualDot(i,ShowBuff_sin[i],0x00);//清除上次的点
			OLEDPrintDualDot(i,y,0x88);//画本次的点
			ShowBuff_sin[i]=y;
			osThreadYield();
			//DelayUs(20);//让出cpu
		}
	}
}
void showCos()
{
	uint8_t index;
	for(uint8_t LineIndex=0;LineIndex<LineLen;LineIndex++){
		for(uint8_t i=LineLen;i>0;)
		{
			i--;
			index=(i+LineIndex+1)%LineLen;
			uint8_t y=DotBuff_cos[index];
			OLEDPrintDualDot(i,ShowBuff_cos[i],0x00);//清除上次的点
			OLEDPrintDualDot(i,y,0x88);//画本次的点
			ShowBuff_cos[i]=y;
			osThreadYield();
			//DelayUs(20);//让出cpu
		}
	}
	
}
void showSinPi()
{
	uint8_t index;
	for(uint8_t LineIndex=0;LineIndex<LineLen;LineIndex++){
		for(uint8_t i=LineLen;i>0;)
		{
			i--;
			index=(i+LineIndex+1)%LineLen;
			uint8_t y=DotBuff_sinPi[index];
			OLEDPrintDualDot(i,ShowBuff_sinPi[i],0x00);//清除上次的点
			OLEDPrintDualDot(i,y,0x88);//画本次的点
			ShowBuff_sinPi[i]=y;
			osThreadYield();
			//DelayUs(20);//让出cpu
		}
	}
}



void USART2_IRQHandler(void)
{
	static uint8_t _bufCnt = 0;//帧长度计数
	
	if(USART2->ISR & USART_ISR_RXNE)//接收到数据
	{
		Usart2Buf[Usart2BufMun][_bufCnt] = USART2->RDR;
		_bufCnt++;//帧长度计数递增
		Usart2BufFlag = 0;//接收标志为0表示该帧未接收完成
	}
	
	if(USART2->ISR & USART_ISR_IDLE)//数据帧结束
	{
		uint8_t sum = 0;
		
		for(uint8_t cnt = 0;cnt < _bufCnt-1;cnt++)
		{
			sum += Usart2Buf[Usart2BufMun][cnt];//校验和
		}
		
		if(sum == Usart2Buf[Usart2BufMun][_bufCnt-1])//校验正确，若校验错误，则序列不增加
		{
			Usart2BufLen[Usart2BufMun] = _bufCnt-1;//记录该帧长度,去掉校验
			Usart2BufMun++;//下一帧
			Usart2BufFlag = 1;//接收完成
		}
		_bufCnt = 0;//从0开始计数
		
		USART2->ICR |= USART_ICR_IDLECF;//清除中断标志
	}
}

void USART2Init(uint32_t buad)
{
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;//使能usart2时钟
	
	GPIOInit(USART2_TX_PORT,USART2_TX_PIN,MODER_AF,OTYPER_PP,OSPEEDR_HIG,PUPDR_PU);//复用功能
	GPIOAF(USART2_TX_PORT,USART2_TX_PIN,1);
	GPIOInit(USART2_RX_PORT,USART2_RX_PIN,MODER_AF,OTYPER_PP,OSPEEDR_HIG,PUPDR_PU);//复用功能
	GPIOAF(USART2_RX_PORT,USART2_RX_PIN,1);
	
	USART2->BRR  = (uint16_t)(36000000.0f /(float32_t)(buad) +0.5f);//buad*BRR=Fck=36M
	USART2->CR1	|= USART_CR1_TE;//TE:发送使能
	USART2->CR1 |= USART_CR1_RE;//RC:接收器使能并开始搜索起始位
	
	USART2->CR1 |= USART_CR1_RXNEIE;//RXNEIE:接收中断使能
	USART2->CR1 |= USART_CR1_IDLEIE;//IDLE:接收帧中断使能
	
	NVIC_SetPriority(USART2_IRQn,0x4);
	NVIC_EnableIRQ(USART2_IRQn);
	
	USART2->CR1 |= USART_CR1_UE;//串口使能
}
