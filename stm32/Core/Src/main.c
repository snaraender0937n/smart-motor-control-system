/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c-lcd.h"
#include <stdio.h>
#include <string.h>
uint32_t motor_start_time = 0;
uint32_t conveyor_start_time = 0;

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
int system_on = 0;
int motor_on = 0;
int conveyor_on = 0;

uint32_t t1_start = 0;
uint32_t t2_start = 0;
uint32_t t3_start = 0;

int t1_done = 0;
int t2_done = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();
  lcd_clear();

  // STARTUP MESSAGE
  lcd_put_cur(0,0);
  lcd_send_string("System Starting");
  lcd_put_cur(1,0);
  lcd_send_string("Please Wait...");

  HAL_Delay(2000);

  lcd_clear();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // ----------- GLOBAL STATIC -----------
      static uint32_t last_sec = 999;
      static int last_state = -1;

      char uart_msg[20];
      char buffer[20];
      uint8_t rx_data;

      // ----------- UART RECEIVE (ESP32 CONTROL) -----------

      if(HAL_UART_Receive(&huart1, &rx_data, 1, 10) == HAL_OK)
      {
          if(rx_data == '1')
              system_on = 1;

          else if(rx_data == '0')
              system_on = 0;
      }

      // ----------- BUTTON INPUT -----------

      if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
      {
          system_on = 1;
          HAL_Delay(200);
      }

      if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
      {
          system_on = 0;

          motor_on = 0;
          conveyor_on = 0;

          t1_done = 0;
          t2_done = 0;

          t1_start = 0;
          t2_start = 0;
          t3_start = 0;

          last_sec = 999;

          HAL_Delay(200);
      }

      // ----------- TIMER SEQUENCE -----------

      if(system_on && !t1_done)
      {
          if(t1_start == 0)
              t1_start = HAL_GetTick();

          if(HAL_GetTick() - t1_start >= 5000)
          {
              t1_done = 1;
              t2_start = HAL_GetTick();
          }
      }

      if(t1_done && !t2_done)
      {
          if(motor_on == 0)
              motor_start_time = HAL_GetTick();

          motor_on = 1;

          if(HAL_GetTick() - t2_start >= 20000)
          {
              t2_done = 1;
              motor_on = 0;
              t3_start = HAL_GetTick();
          }
      }

      if(t2_done)
      {
          if(conveyor_on == 0)
              conveyor_start_time = HAL_GetTick();

          conveyor_on = 1;

          if(HAL_GetTick() - t3_start >= 8000)
          {
              conveyor_on = 0;

              system_on = 0;

              t1_done = 0;
              t2_done = 0;

              t1_start = 0;
              t2_start = 0;
              t3_start = 0;

              last_sec = 999;
          }
      }

      // ----------- OUTPUT -----------

      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, motor_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, conveyor_on ? GPIO_PIN_SET : GPIO_PIN_RESET);

      // ----------- LCD DISPLAY -----------

      if(motor_on && last_state != 1)
      {
          lcd_clear();
          last_state = 1;
          last_sec = 999;
      }
      else if(conveyor_on && last_state != 2)
      {
          lcd_clear();
          last_state = 2;
          last_sec = 999;
      }
      else if(!motor_on && !conveyor_on && last_state != 0)
      {
          lcd_clear();
          last_state = 0;
          last_sec = 999;
      }

      if(motor_on)
      {
          uint32_t sec = (HAL_GetTick() - motor_start_time) / 1000;

          lcd_put_cur(0,0);
          lcd_send_string("Motor ON     ");

          lcd_put_cur(1,0);
          sprintf(buffer, "Time %02lu s", sec);
          lcd_send_string(buffer);

          // UART (send only when time changes)
          if(sec != last_sec)
          {
              snprintf(uart_msg, sizeof(uart_msg), "M%lu\n", (unsigned long)sec);
              HAL_UART_Transmit(&huart1, (uint8_t*)uart_msg, strlen(uart_msg), 100);
              last_sec = sec;
          }
      }
      else if(conveyor_on)
      {
          uint32_t sec = (HAL_GetTick() - conveyor_start_time) / 1000;

          lcd_put_cur(0,0);
          lcd_send_string("Conveyor ON  ");

          lcd_put_cur(1,0);
          sprintf(buffer, "Time %02lu s", sec);
          lcd_send_string(buffer);

          // UART
          if(sec != last_sec)
          {
              snprintf(uart_msg, sizeof(uart_msg), "C%lu\n", (unsigned long)sec);
              HAL_UART_Transmit(&huart1, (uint8_t*)uart_msg, strlen(uart_msg), 100);
              last_sec = sec;
          }
      }
      else
      {
          lcd_put_cur(0,0);
          lcd_send_string("System Idle  ");

          lcd_put_cur(1,0);
          lcd_send_string("             ");

          // UART (send once)
          if(last_sec != 1000)
          {
              snprintf(uart_msg, sizeof(uart_msg), "I\n");
              HAL_UART_Transmit(&huart1, (uint8_t*)uart_msg, strlen(uart_msg), 100);
              last_sec = 1000;
          }
      }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
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

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA6 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
