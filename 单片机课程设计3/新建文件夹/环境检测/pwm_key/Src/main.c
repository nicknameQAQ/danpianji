/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"

uint16_t pwm_value = 0;
uint8_t flag=0;
uint8_t humi=35;
uint8_t temp=25;
uint8_t mode1=0;
uint8_t menu=0;
uint8_t key_num=0;
uint16_t num1=0;
uint16_t num2=0;
uint16_t pwmt=0;
uint16_t pwmh=0;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

void write_guanbiao(uint8_t x,uint8_t y,uint8_t date)
{
	if(y==0)
	LCD1602_WriteCmd(0x80+x);
	else
	LCD1602_WriteCmd(0x80+0x40+x);
	if(date == 1)
	LCD1602_WriteCmd(0x0f); //光标闪烁
	else
	LCD1602_WriteCmd(0x0c); //关闭光标
}

uint8_t MAX(uint8_t x,uint8_t y)
{
	uint8_t max;
	if(x>y) max=x;
	else max=y;
	return max;
}

//void key_with(void)
//{
//	key_num=keyscan(0);
//	if(key_num==1)
//	{
//		mode1++;
//		LCD1602_WriteCmd(0x01);
//		if(mode1>1)
//		{
//			mode1 = 0;
//			menu = 0;
//			LCD1602_WriteCmd(0x0c);
//		}
//		key_num=0;
//	}
//	if(mode1==0)
//	{
//		LCD1602_ShowStr(0,0,"H   T   FAN    ",15);
//		LCD1602_ShowChar(0,1,(uint8_t)humidity/10+0x30);
//		LCD1602_ShowChar(1,1,(uint8_t)humidity%10+0x30);
//		LCD1602_ShowChar(4,1,(uint8_t)temperature/10+0x30);
//		LCD1602_ShowChar(5,1,(uint8_t)temperature%10+0x30);
//		if(pwm_value==0)	LCD1602_ShowStr(8,1,"OFF",3);
//		else							LCD1602_ShowStr(8,1,"ON ",3);
//	}
//	if(key_num==5)
//	{
//		mode1 = 0;
//		menu = 0;
//		LCD1602_WriteCmd(0x01);
//		LCD1602_WriteCmd(0x0c);
//		key_num=0;
//	}
//	if(mode1==1)
//	{
//		LCD1602_ShowStr(0,0,"SET   H   T",11);
//		LCD1602_ShowChar(6,1, humi/10+0x30);
//		LCD1602_ShowChar(7,1, humi%10+0x30);
//		LCD1602_ShowChar(10,1,temp/10+0x30);
//		LCD1602_ShowChar(11,1,temp%10+0x30);
//		if(menu==0)
//		{
//			if(key_num==3)
//			{
//				humi++;
//				key_num=0;
//			}
//			if(key_num==4)
//			{
//				humi--;
//				key_num=0;
//			}
//			if(key_num==2)
//			{
//				menu++;
//				if(menu>1) menu = 0;
//			}
//			LCD1602_ShowChar(6,1, humi/10+0x30);
//			LCD1602_ShowChar(7,1, humi%10+0x30);
//			write_guanbiao(6,0,1);
//		}
//		if(menu==1)
//		{
//			if(key_num==3)
//			{
//				temp++;
//				key_num=0;
//			}
//			if(key_num==4) 
//			{
//				temp--;
//				key_num=0;
//			}
//			if(key_num==2)
//			{
//				menu++;
//				if(menu>1) menu = 0;
//			}
//			LCD1602_ShowChar(10,1,temp/10+0x30);
//			LCD1602_ShowChar(11,1,temp%10+0x30);
//			write_guanbiao(10,0,1);
//		}
//	}
//}

void key_system(void)
{
	key_num=keyscan(0);
	switch(key_num)
	{
		case 1:
			mode1++;
			flag=1;
			LCD1602_WriteCmd(0x01);
			if(mode1>1)
			{
				mode1 = 0;
				menu = 0;
				LCD1602_WriteCmd(0x0c);
			}
			break;
		case 2:
			if(mode1==0) break;
			else
			{
				flag=1;
				menu++;
				if(menu>1)
				{
					menu = 0;
				}
			}
			break;
		case 3:
			if(mode1==0) break;
			flag=1;
			if(menu==0) humi++;
			else				temp++;
			break;
		case 4:
			if(mode1==0) break;
			flag=1;
			if(menu==0) humi--;
			else				temp--;
			break;
		case 5:
			if(mode1==0) break;
			else
			{
				mode1 = 0;
				menu = 0;
				LCD1602_WriteCmd(0x01);
				LCD1602_WriteCmd(0x0c);
			}
			break;
		default: break;
	}
}

uint16_t FAN_control(void)
{
	uint16_t pwm;
	if(temperature<temp && humidity<humi)	 pwm=0;
	if(temperature>36 && temperature>temp) pwm=100;
	if(humidity>80 && humidity>humi)       pwm=100;
	if(temperature>=temp || humidity>=humi)
	{
		if(humidity<=80 && humidity>=humi)
		{
			num1=(uint8_t)humidity-humi;
			num2=80-humi;
			pwmh=num1*100/num2;
		}
		if(temperature<=36 && temperature>=temp)
		{
			num1=(uint8_t)temperature-temp;
			num2=36-temp;
			pwmh=num1*100/num2;
		}
		pwm=MAX(pwmh,pwmt);
	}
	if(temperature>36 && temperature>temp) pwm=100;
	if(humidity>80 && humidity>humi)       pwm=100;
	return pwm;
}
/* USER CODE END PTD */

/* USER CODE BEGIN PFP */
void SystemClock_Config_FW(void);
void SystemClock_Config(void);
/* USER CODE END PFP */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  HAL_Init();
  SystemClock_Config_FW();
	SystemClock_Config();
  MX_GPIO_Init();
	LCD1602_Init();
	LCD1602_ShowStr(0,0,"INT...",6);
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
  HAL_Delay(100);
  /* USER CODE END 1 */
  while (1)
  {
    /* USER CODE BEGIN 3 */
//		key_with();
		DHT11_Show_Data();
		get_ADValue();
		do
		{
			key_system();
			if(mode1==0)
			{
				LCD1602_ShowStr(0,0,"H   T   FAN    ",15);
				LCD1602_ShowChar(0,1,(uint8_t)humidity/10+0x30);
				LCD1602_ShowChar(1,1,(uint8_t)humidity%10+0x30);
				LCD1602_ShowChar(4,1,(uint8_t)temperature/10+0x30);
				LCD1602_ShowChar(5,1,(uint8_t)temperature%10+0x30);
				if(pwm_value==0)	LCD1602_ShowStr(8,1,"OFF",3);
				else							LCD1602_ShowStr(8,1,"ON ",3);
			}
			else
			{
				if(menu==0&&flag==1)
				{
					LCD1602_ShowStr(0,0,"SET   H   T",11);
					LCD1602_ShowChar(6,1, humi/10+0x30);
					LCD1602_ShowChar(7,1, humi%10+0x30);
					LCD1602_ShowChar(10,1,temp/10+0x30);
					LCD1602_ShowChar(11,1,temp%10+0x30);
					write_guanbiao(6,0,1);
					flag=0;
					HAL_Delay(200);
				}
				else if(flag==1)
				{
					LCD1602_ShowStr(0,0,"SET   H   T",11);
					LCD1602_ShowChar(6,1, humi/10+0x30);
					LCD1602_ShowChar(7,1, humi%10+0x30);
					LCD1602_ShowChar(10,1,temp/10+0x30);
					LCD1602_ShowChar(11,1,temp%10+0x30);
					write_guanbiao(10,0,1);
					flag=0;
					HAL_Delay(200);
				}
			}
		}while(mode1==1);
		pwm_value=FAN_control();
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,pwm_value);
		HAL_Delay(10);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
	
}

void SystemClock_Config_FW(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSIState = RCC_HSE_ON;
//  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
