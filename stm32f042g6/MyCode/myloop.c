#include "myloop.h"
#include <math.h>
#include "cmsis_os2.h"
#include <stdio.h>



#define USART2_TX_PIN			2
#define USART2_TX_PORT			GPIOA

#define USART2_RX_PIN			3
#define USART2_RX_PORT			GPIOA

uint8_t Usart2Buf[0xFF][0x08];//���ݻ���
uint8_t Usart2BufLen[0xff];//����֡����
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
	while(!(USART2->ISR & USART_ISR_TXE));//�ȴ���������
	USART2->TDR = (uint8_t)(ch);//ѭ������,ֱ���������
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
			OLEDPrintDualDot(i,ShowBuff_sin[i],0x00);//����ϴεĵ�
			OLEDPrintDualDot(i,y,0x88);//�����εĵ�
			ShowBuff_sin[i]=y;
			osThreadYield();
			//DelayUs(20);//�ó�cpu
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
			OLEDPrintDualDot(i,ShowBuff_cos[i],0x00);//����ϴεĵ�
			OLEDPrintDualDot(i,y,0x88);//�����εĵ�
			ShowBuff_cos[i]=y;
			osThreadYield();
			//DelayUs(20);//�ó�cpu
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
			OLEDPrintDualDot(i,ShowBuff_sinPi[i],0x00);//����ϴεĵ�
			OLEDPrintDualDot(i,y,0x88);//�����εĵ�
			ShowBuff_sinPi[i]=y;
			osThreadYield();
			//DelayUs(20);//�ó�cpu
		}
	}
}



void USART2_IRQHandler(void)
{
	static uint8_t _bufCnt = 0;//֡���ȼ���
	
	if(USART2->ISR & USART_ISR_RXNE)//���յ�����
	{
		Usart2Buf[Usart2BufMun][_bufCnt] = USART2->RDR;
		_bufCnt++;//֡���ȼ�������
		Usart2BufFlag = 0;//���ձ�־Ϊ0��ʾ��֡δ�������
	}
	
	if(USART2->ISR & USART_ISR_IDLE)//����֡����
	{
		uint8_t sum = 0;
		
		for(uint8_t cnt = 0;cnt < _bufCnt-1;cnt++)
		{
			sum += Usart2Buf[Usart2BufMun][cnt];//У���
		}
		
		if(sum == Usart2Buf[Usart2BufMun][_bufCnt-1])//У����ȷ����У����������в�����
		{
			Usart2BufLen[Usart2BufMun] = _bufCnt-1;//��¼��֡����,ȥ��У��
			Usart2BufMun++;//��һ֡
			Usart2BufFlag = 1;//�������
		}
		_bufCnt = 0;//��0��ʼ����
		
		USART2->ICR |= USART_ICR_IDLECF;//����жϱ�־
	}
}

void USART2Init(uint32_t buad)
{
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;//ʹ��usart2ʱ��
	
	GPIOInit(USART2_TX_PORT,USART2_TX_PIN,MODER_AF,OTYPER_PP,OSPEEDR_HIG,PUPDR_PU);//���ù���
	GPIOAF(USART2_TX_PORT,USART2_TX_PIN,1);
	GPIOInit(USART2_RX_PORT,USART2_RX_PIN,MODER_AF,OTYPER_PP,OSPEEDR_HIG,PUPDR_PU);//���ù���
	GPIOAF(USART2_RX_PORT,USART2_RX_PIN,1);
	
	USART2->BRR  = (uint16_t)(36000000.0f /(float32_t)(buad) +0.5f);//buad*BRR=Fck=36M
	USART2->CR1	|= USART_CR1_TE;//TE:����ʹ��
	USART2->CR1 |= USART_CR1_RE;//RC:������ʹ�ܲ���ʼ������ʼλ
	
	USART2->CR1 |= USART_CR1_RXNEIE;//RXNEIE:�����ж�ʹ��
	USART2->CR1 |= USART_CR1_IDLEIE;//IDLE:����֡�ж�ʹ��
	
	NVIC_SetPriority(USART2_IRQn,0x4);
	NVIC_EnableIRQ(USART2_IRQn);
	
	USART2->CR1 |= USART_CR1_UE;//����ʹ��
}
