#include "mybsplib.h"

uint32_t CPU_ID[3];

void GetChipID(void)
{
	CPU_ID[0] = *(__IO uint32_t *)(ID_ADD);							//高字节
	CPU_ID[1] = *(__IO uint32_t *)(ID_ADD + ID_OFFSET);
	CPU_ID[2] = *(__IO uint32_t *)(ID_ADD + ID_OFFSET + ID_OFFSET);	//低字节
}

void GPIOInit(GPIO_TypeDef* GPIOx,uint8_t pin,uint8_t modeR,uint8_t otypeR,uint8_t ospeedR,uint8_t pupdR)
{
	GPIOx->MODER	&=	(uint32_t)(~(0x03<<(pin*0x02)));	//先清除原来的设置
	GPIOx->MODER	|=	(uint32_t)(modeR<<(pin*0x02));		//设置新的模式 
	
	GPIOx->OTYPER	&=	(uint32_t)(~(0x01<<pin));			//清除原来的设置
	GPIOx->OTYPER	|=	(uint32_t)(otypeR<<pin);			//设置新的输出模式
	
	GPIOx->OSPEEDR	&=	(uint32_t)(~(0x03<<(pin*0x02)));	//清除原来的设置
	GPIOx->OSPEEDR	|=	(uint32_t)(ospeedR<<(pin*0x02));	//设置新的速度值  
	
	GPIOx->PUPDR	&=	(uint32_t)(~(0x03<<(pin*0x02)));	//先清除原来的设置
	GPIOx->PUPDR	|=	(uint32_t)(pupdR<<(pin*0x02));		//设置新的上下拉
}


void GPIOAF(GPIO_TypeDef* GPIOx,uint8_t pin,uint8_t AFx)
{  
	GPIOx->AFR[pin>>3]	&=	(uint32_t)(~(0x0F<<((pin&0x07)*0x04)));
	GPIOx->AFR[pin>>3]	|=	(uint32_t)(AFx<<((pin&0x07)*0x04));
}

void GPIOOutput(GPIO_TypeDef* GPIOx,uint8_t pin,uint8_t stat)
{
	if(stat){GPIOx->BSRR|= (uint32_t)(1<<pin);}
	else	{GPIOx->BRR	|= (uint32_t)(1<<pin);}
}

void GPIOOutputLow(GPIO_TypeDef* GPIOx,uint8_t pin)
{
	GPIOx->BRR	|= (uint32_t)(1<<pin);
}

void GPIOOutputHig(GPIO_TypeDef* GPIOx,uint8_t pin)
{
	GPIOx->BSRR	|= (uint32_t)(1<<pin);
}

uint8_t GPIORead(GPIO_TypeDef* GPIOx,uint8_t pin)
{
	return (uint8_t)((GPIOx->IDR & (uint32_t)(1<<pin))>>pin);
}

void SWJDisable(void)
{
	//设置为下拉输入
	GPIOInit(GPIOA,13,0,0,0,PUPDR_PD);
	GPIOInit(GPIOA,14,0,0,0,PUPDR_PD);
}

static void RCCReset(void)
{
	#ifdef STM32F042x6
	RCC->APB1RSTR		|=	RCC_APB1RSTR_TIM2RST	|
							RCC_APB1RSTR_WWDGRST	|
							RCC_APB1RSTR_SPI2RST	|
							RCC_APB1RSTR_USART2RST	|
							RCC_APB1RSTR_I2C1RST	|
							RCC_APB1RSTR_USBRST		|
							RCC_APB1RSTR_CANRST		|
							RCC_APB1RSTR_PWRRST		;
	
	RCC->APB2RSTR		|=	RCC_APB2RSTR_SYSCFGRST	|
							RCC_APB2RSTR_TIM1RST	|
							RCC_APB2RSTR_USART1RST	|
							RCC_APB2RSTR_TIM16RST	|
							RCC_APB2RSTR_TIM17RST	;
										
	RCC->APB1RSTR	&=	~(	RCC_APB1RSTR_TIM2RST	|
							RCC_APB1RSTR_SPI2RST	|
							RCC_APB1RSTR_USART2RST	|
							RCC_APB1RSTR_I2C1RST	|
							RCC_APB1RSTR_USBRST		|
							RCC_APB1RSTR_CANRST		|
							RCC_APB1RSTR_PWRRST		);
	
	RCC->APB2RSTR	&=	~(	RCC_APB2RSTR_SYSCFGRST	|
							RCC_APB2RSTR_TIM1RST	|
							RCC_APB2RSTR_USART1RST	|
							RCC_APB2RSTR_TIM16RST	|
							RCC_APB2RSTR_TIM17RST	);
	
	RCC->AHBENR		|=		RCC_AHBENR_GPIOAEN		|
							RCC_AHBENR_GPIOBEN		;
	#endif
	
	#ifdef STM32F302x8
	RCC->APB1RSTR		|=	RCC_APB1RSTR_TIM2RST	|
							RCC_APB1RSTR_TIM6RST	|
							RCC_APB1RSTR_WWDGRST	|
							RCC_APB1RSTR_SPI2RST	|
							RCC_APB1RSTR_SPI3RST	|
							RCC_APB1RSTR_USART2RST	|
							RCC_APB1RSTR_USART3RST	|
							RCC_APB1RSTR_I2C1RST	|
							RCC_APB1RSTR_I2C2RST	|
							RCC_APB1RSTR_USBRST		|
							RCC_APB1RSTR_CANRST		|
							RCC_APB1RSTR_PWRRST		|
							RCC_APB1RSTR_DAC1RST	|
							RCC_APB1RSTR_I2C3RST	;
	
	RCC->APB2RSTR		|=	RCC_APB2RSTR_SYSCFGRST	|
							RCC_APB2RSTR_TIM1RST	|
							RCC_APB2RSTR_USART1RST	|
							RCC_APB2RSTR_TIM15RST	|
							RCC_APB2RSTR_TIM16RST	|
							RCC_APB2RSTR_TIM17RST	;
										
	RCC->APB1RSTR	&=	~(	RCC_APB1RSTR_TIM2RST	|
							RCC_APB1RSTR_TIM6RST	|
							RCC_APB1RSTR_WWDGRST	|
							RCC_APB1RSTR_SPI2RST	|
							RCC_APB1RSTR_SPI3RST	|
							RCC_APB1RSTR_USART2RST	|
							RCC_APB1RSTR_USART3RST	|
							RCC_APB1RSTR_I2C1RST	|
							RCC_APB1RSTR_I2C2RST	|
							RCC_APB1RSTR_USBRST		|
							RCC_APB1RSTR_CANRST		|
							RCC_APB1RSTR_PWRRST		|
							RCC_APB1RSTR_DAC1RST	|
							RCC_APB1RSTR_I2C3RST	);
	
	RCC->APB2RSTR	&=	~(	RCC_APB2RSTR_SYSCFGRST	|
							RCC_APB2RSTR_TIM1RST	|
							RCC_APB2RSTR_USART1RST	|
							RCC_APB2RSTR_TIM15RST	|
							RCC_APB2RSTR_TIM16RST	|
							RCC_APB2RSTR_TIM17RST	);
	
	RCC->AHBENR		|=		RCC_AHBENR_GPIOAEN		|
							RCC_AHBENR_GPIOBEN		;
	#endif
}

void IWDGFeed(void)
{
	IWDG->KR = 0x0000aaaa;									   
}

static void IWDGInit(uint16_t cycleMs) 
{
	IWDG->KR = 0x00005555;		//解锁对IWDG->PR和IWDG->RLR的写
	
	/* 使用40Khz LSI时钟，Tout = ((4×2^prer) × rlr) /40	*/
	IWDG->PR = IWDG_PR_PR_2;	//设置分频系数，分频数:0~7
	IWDG->RLR = (uint32_t)((float32_t)cycleMs /1.6f);  	//自动重装载值,0~0XFFF
	
	IWDGFeed();
	IWDG->KR = 0x0000cccc;		//使能看门狗
}

#define DELAY_OFFSET 4//微调时钟误差

static uint8_t fac_us=0;

static void DelayInit(void)
{
 	SysTick->CTRL &= ~(1<<2);//SYSTICK使用外部时钟源
	fac_us = (uint8_t)(SystemCoreClock/8000000);//SysTick=HCLK/8
}

void DelayUs(uint16_t nus)
{
	uint32_t temp;
	SysTick->LOAD	=(uint32_t)(nus*fac_us-DELAY_OFFSET);
	SysTick->VAL	=0x00;//清空计数器
	SysTick->CTRL	=0x01 ;//开始倒数
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达
	
	SysTick->CTRL	=0x00;//关闭计数器
	SysTick->VAL	=0X00;//清空计数器
}

void DelayMs(uint16_t nus)
{
	uint32_t temp;
	SysTick->LOAD	=(uint32_t)(nus*fac_us*1000-DELAY_OFFSET);
	SysTick->VAL	=0x00;//清空计数器
	SysTick->CTRL	=0x01;//开始倒数
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达
	
	SysTick->CTRL	=0x00;//关闭计数器
	SysTick->VAL 	=0X00;//清空计数器
}

#ifdef STM32F302x8
/*
  *=============================================================================
  *-----------------------------------------------------------------------------
  *        System Clock source                    | PLL (HSE)
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 72000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 72000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 2
  *-----------------------------------------------------------------------------
  *        HSE Frequency(Hz)                      | 8000000
  *----------------------------------------------------------------------------
  *        PLLMUL                                 | 9
  *-----------------------------------------------------------------------------
  *        PREDIV                                 | 1
  *-----------------------------------------------------------------------------
  *        USB Clock                              | DISABLE
  *-----------------------------------------------------------------------------
  *        Flash Latency(WS)                      | 2
  *-----------------------------------------------------------------------------
  *        Prefetch Buffer                        | ON
  *-----------------------------------------------------------------------------
  *=============================================================================
*/
#if !defined  (HSE_STARTUP_TIMEOUT) 
#define HSE_STARTUP_TIMEOUT  ((uint16_t)0x5000)   /*!< Time out for HSE start up */
#endif /* HSE_STARTUP_TIMEOUT */

static void SetSysClock(void)
{
	__IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
	/* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
	/* Enable HSE */    
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);
	
	/* Wait till HSE is ready and if Time out is reached exit */
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;  
	}while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	if ((RCC->CR & RCC_CR_HSERDY) != RESET)
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		HSEStatus = (uint32_t)0x00;
	}  
	
	if (HSEStatus == (uint32_t)0x01)
	{
		/* Enable Prefetch Buffer and set Flash Latency */
		FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;
		/* HCLK = SYSCLK */
		RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
		/* PCLK2 = HCLK */
		RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
		/* PCLK1 = HCLK /2 */
		RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

		/*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL));
		RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL9);

		/* Enable PLL */
		RCC->CR |= RCC_CR_PLLON;

		/* Wait till PLL is ready */
		while((RCC->CR & RCC_CR_PLLRDY) == 0){}

		/* Select PLL as system clock source */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

		/* Wait till PLL is used as system clock source */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL){}
	}
	else
	{
		while(1);
		/* If HSE fails to start-up, the application will have wrong clock 
			configuration. User can add here some code to deal with this error */
	}
}

static void CSSInit(void)
{
	RCC->CR  |= RCC_CR_CSSON;
	RCC->CIR |= RCC_CIR_CSSF;
}

/*
==========================================================================================================================
 NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  |       Description
==========================================================================================================================
NVIC_PRIORITYGROUP_0  |                0                  |            0-15             | 0 bits for pre-emption priority
											|                                   |                             | 4 bits for subpriority
--------------------------------------------------------------------------------------------------------------------------
NVIC_PRIORITYGROUP_1  |                0-1                |            0-7              | 1 bits for pre-emption priority
											|                                   |                             | 3 bits for subpriority
--------------------------------------------------------------------------------------------------------------------------
NVIC_PRIORITYGROUP_2  |                0-3                |            0-3              | 2 bits for pre-emption priority
											|                                   |                             | 2 bits for subpriority
--------------------------------------------------------------------------------------------------------------------------
NVIC_PRIORITYGROUP_3  |                0-7                |            0-1              | 3 bits for pre-emption priority
											|                                   |                             | 1 bits for subpriority
--------------------------------------------------------------------------------------------------------------------------
NVIC_PRIORITYGROUP_4  |                0-15               |            0                | 4 bits for pre-emption priority
											|                                   |                             | 0 bits for subpriority
==========================================================================================================================
*/
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bits for pre-emption priority
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bits for pre-emption priority
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority
                                                                 1 bits for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority
                                                                 0 bits for subpriority */
void NVICSet(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t prioritygroup = 0x00; 
  prioritygroup = NVIC_GetPriorityGrouping();
  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}
#endif

void BspInit(void)
{
	#ifdef STM32F302x8
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	SetSysClock();//HSE=8M,SYSCLK=72M,HCLK(AHB)=72M,PCLK1(APB1)=36M,PCLK2(APB2)=72M
	#endif
	SystemCoreClockUpdate();//时钟值更新
	RCCReset();
	DelayInit();
	
	IWDGInit(2000);//看门狗复位周期2000ms
}
