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
 * 96位芯片ID
 */
void GetChipID(void);

/*
 * GPIO定义
 */
#define MODER_IN		0x0//普通输入模式
#define MODER_OUT		0x1//普通输出模式
#define MODER_AF		0x2//AF功能模式
#define MODER_AIN		0x3//模拟输入模式

#define OTYPER_PP		0x0//推挽输出
#define OTYPER_OD		0x1//开漏输出

#define OSPEEDR_LOW		0x0//GPIO速度2Mhz
#define OSPEEDR_MID		0x1//GPIO速度25Mhz
#define OSPEEDR_HIG		0x3//GPIO速度50Mhz

#define PUPDR_NONE		0x0//不带上下拉
#define PUPDR_PU		0x1//上拉
#define PUPDR_PD		0x2//下拉
#define PUPDR_RES		0x3//保留 

/*
 * GPIOx:GPIOA~GPIOI.
 * pin:0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推。比如0X0101,代表同时设置Px0和Px8。
 * modeR:0~3;模式选择,0,输入(系统复位默认状态);1,普通输出;2,复用功能;3,模拟输入。
 * otypeR:0/1;输出类型选择,0,推挽输出;1,开漏输出。
 * ospeedR:0~3;输出速度设置,x0:low;01:mid;11:high;
 * pupdR:0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留。
 * 注意:在输入模式(普通输入/模拟输入)下,OTYPE和OSPEED参数无效。
 */
void GPIOInit(GPIO_TypeDef* GPIOx,uint8_t pin,uint8_t modeR,uint8_t otypeR,uint8_t ospeedR,uint8_t pupdR);

/*
 * GPIO复用设置
 * GPIOx:GPIOA~GPIOI。
 * pin:0~15,代表IO引脚编号。
 * AFx:0~15,代表AF0~AF15。
 */
void GPIOAF(GPIO_TypeDef* GPIOx,uint8_t pin,uint8_t AFx);

void GPIOOutput(GPIO_TypeDef* GPIOx,uint8_t pin,uint8_t stat);//输出0/1
void GPIOOutputLow(GPIO_TypeDef* GPIOx,uint8_t pin);//输出低
void GPIOOutputHig(GPIO_TypeDef* GPIOx,uint8_t pin);//输出高
uint8_t GPIORead(GPIO_TypeDef* GPIOx,uint8_t pin);//输入0/1

//禁用SW（慎用，会导致SW不能烧进程序）
void SWJDisable(void);

/*
 * 喂狗
 */
void IWDGFeed(void);

/*
 * 延时
 */
void DelayUs(uint16_t nus);
void DelayMs(uint16_t nus);

#ifdef STM32F302x8
void NVICSet(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
#endif

void BspInit(void);


#endif
