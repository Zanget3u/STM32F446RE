#include "stm32f446xx.h"
#include "system_stm32f4xx.h"

#include "AL_GPIO.h"
#include "AL_Utility.h"
#include "AL_EXTI.h"

extern 	void SystemCoreClockUpdate(void);
		void SysTick_Handler(void);

uint16_t tick = 0;

int main(void)
{
	__disable_irq();

	//Configure SysTick
	SystemCoreClockUpdate(); //ARM-Function => sets SysTick depending on System-Clock
	SysTick_Config(SystemCoreClock / 1000); //SysTick = 1ms

	//Activate clocks
	AL_gpioInitPort(GPIOA);
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//Configure GPIOA
	AL_gpioSelectPinMode(GPIOA, PIN0, INPUT);
	AL_gpioSelectPinMode(GPIOA, PIN1, INPUT);
	AL_gpioSelectPinMode(GPIOA, PIN4, OUTPUT);
	AL_gpioSelectPinMode(GPIOA, PIN5, OUTPUT);
	AL_gpioSetOutputType(GPIOA, PIN0, PUSHPULL);
	AL_gpioSetOutputType(GPIOA, PIN1, PUSHPULL);
	AL_gpioSetOutputType(GPIOA, PIN4, PUSHPULL);
	AL_gpioSetOutputType(GPIOA, PIN5, PUSHPULL);
	AL_gpioSelectPushPullType(GPIOA, PIN0, NO_PULLUP_PULLDOWN);
	AL_gpioSelectPushPullType(GPIOA, PIN1, NO_PULLUP_PULLDOWN);
	AL_gpioSelectPushPullType(GPIOA, PIN4, NO_PULLUP_PULLDOWN);
	AL_gpioSelectPushPullType(GPIOA, PIN5, NO_PULLUP_PULLDOWN);

	//Configure EXTI
	AL_extiInit();
	AL_extiConfigIrq(GPIOA, PIN0);
	AL_extiConfigIrq(GPIOA, PIN1);
	AL_extiEnableIrq(EXTI_PIN0);
	AL_extiEnableIrq(EXTI_PIN1);
	AL_extiSetTriggerEdge(EXTI_PIN0, FALLING_EDGE);
	AL_extiSetTriggerEdge(EXTI_PIN1, FALLING_EDGE);

	NVIC_EnableIRQ(EXTI0_IRQn); // EXTI0: Aktivieren
	NVIC_EnableIRQ(EXTI1_IRQn); // EXTI1: Aktivieren

	__enable_irq();

	while(1)
	{
		if(tick > 199)
		{
			AL_gpioTogglePin(GPIOA, PIN5); //Keep alive signal
			tick = 0;
		}
	}
}

void EXTI0_IRQHandler(void)
{
    AL_gpioSetPin(GPIOA, PIN4);
    AL_extiResetPending(EXTI_PIN0);
}

void EXTI1_IRQHandler(void)
{
	AL_gpioResetPin(GPIOA, PIN4);
    AL_extiResetPending(EXTI_PIN1);
}

void SysTick_Handler(void)
{
	++tick;
}
