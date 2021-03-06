#include "safetyHandler.h"
#include <stm32f4xx_hal.h>
#include <gpio.h>
#include <stdio.h>
#include "liftHandler.h"
#include "tim.h"
#include "dac.h"

volatile struct Emergency emergency = { 0, 0, 0 };
float overweightValue = 5; //The value of the weight in tons:  1 ton = 1000 kg. Max value = 10.0 tons
float valVolt = 1.5;    //Range: 0 - 3 volts If overweight 5 -> 1.5 volts
uint8_t valByte;

void initSafetyPins()
{
	// Init Button & Overweight 
	GPIO_InitTypeDef port;

	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	port.Pin = GPIO_PIN_1 | GPIO_PIN_3;
	port.Mode = GPIO_MODE_IT_RISING;
	port.Pull = GPIO_PULLDOWN;
	
	HAL_GPIO_Init(GPIOA, &port);	

	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
	
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	
	// Init Door
	port.Pin = GPIO_PIN_2;
	port.Mode = GPIO_MODE_INPUT;
	port.Pull = GPIO_PULLDOWN;
	
	HAL_GPIO_Init(GPIOA, &port);	
	
	//Init overweight voltage reference
	
	valVolt = 3 * overweightValue / 10;	
	valByte =(uint8_t)((valVolt / 3) * 255);
	
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1,DAC_ALIGN_8B_R, valByte);
}

void handleEmergency()
{
	emergency.door = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
	if (!emergency.door) lockLifter();
	
	if (emergency.stop || emergency.overweight)
	{
		lockLifter();

		HAL_GPIO_WritePin(GPIOD, emergencyLightPins, GPIO_PIN_SET);
		HAL_Delay(250);
		HAL_GPIO_WritePin(GPIOD, emergencyLightPins, GPIO_PIN_RESET);
		HAL_Delay(250);

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == RESET ) emergency.stop = 0; 
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == RESET ) emergency.overweight = 0; 
	} 
	else if (emergency.door)
		unlockLifter();
}

void lockLifter()
{	
	stopMotor();
  HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	HAL_TIM_Base_Stop_IT(&htim7);
	
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
}

void unlockLifter()
{
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_TIM_Base_Start_IT(&htim7);
}
