#ifndef __MYBSPLIB_H
#define __MYBSPLIB_H

#include "arm_math.h"

/*
 * for stm32f042g6
 */
#ifdef STM32F042x6
#include "stm32f0xx.h"

#define ID_ADD 0x1FFFF7AC
#define ID_OFFSET 0x4
#endif

/*
 * for stm32f302k8
 */
#ifdef STM32F302x8
#include "stm32f3xx.h"

#define ID_ADD 0x1FFFF7AC
#define ID_OFFSET 0x4
#endif

/*
 * 96λоƬID
 */
void GetChipID(void);

/*
 * GPIO����
 */
#define MODER_IN		0x0//��ͨ����ģʽ
#define MODER_OUT		0x1//��ͨ���ģʽ
#define MODER_AF		0x2//AF����ģʽ
#define MODER_AIN		0x3//ģ������ģʽ

#define OTYPER_PP		0x0//�������
#define OTYPER_OD		0x1//��©���

#define OSPEEDR_LOW		0x0//GPIO�ٶ�2Mhz
#define OSPEEDR_MID		0x1//GPIO�ٶ�25Mhz
#define OSPEEDR_HIG		0x3//GPIO�ٶ�50Mhz

#define PUPDR_NONE		0x0//����������
#define PUPDR_PU		0x1//����
#define PUPDR_PD		0x2//����
#define PUPDR_RES		0x3//���� 

/*
 * GPIOx:GPIOA~GPIOI.
 * pin:0X0000~0XFFFF,λ����,ÿ��λ����һ��IO,��0λ����Px0,��1λ����Px1,�������ơ�����0X0101,����ͬʱ����Px0��Px8��
 * modeR:0~3;ģʽѡ��,0,����(ϵͳ��λĬ��״̬);1,��ͨ���;2,���ù���;3,ģ�����롣
 * otypeR:0/1;�������ѡ��,0,�������;1,��©�����
 * ospeedR:0~3;����ٶ�����,x0:low;01:mid;11:high;
 * pupdR:0~3:����������,0,����������;1,����;2,����;3,������
 * ע��:������ģʽ(��ͨ����/ģ������)��,OTYPE��OSPEED������Ч��
 */
void GPIOInit(GPIO_TypeDef* GPIOx,uint8_t pin,uint8_t modeR,uint8_t otypeR,uint8_t ospeedR,uint8_t pupdR);

/*
 * GPIO��������
 * GPIOx:GPIOA~GPIOI��
 * pin:0~15,����IO���ű�š�
 * AFx:0~15,����AF0~AF15��
 */
void GPIOAF(GPIO_TypeDef* GPIOx,uint8_t pin,uint8_t AFx);

void GPIOOutput(GPIO_TypeDef* GPIOx,uint8_t pin,uint8_t stat);//���0/1
void GPIOOutputLow(GPIO_TypeDef* GPIOx,uint8_t pin);//�����
void GPIOOutputHig(GPIO_TypeDef* GPIOx,uint8_t pin);//�����
uint8_t GPIORead(GPIO_TypeDef* GPIOx,uint8_t pin);//����0/1

//����SW�����ã��ᵼ��SW�����ս�����
void SWJDisable(void);

/*
 * ι��
 */
void IWDGFeed(void);

/*
 * ��ʱ
 */
void DelayUs(uint16_t nus);
void DelayMs(uint16_t nus);

#ifdef STM32F302x8
void NVICSet(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
#endif

void BspInit(void);


#endif
