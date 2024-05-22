/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <stdio.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "motor.h"

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
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

//static volatile char msg[100] = "";
static volatile uint8_t rcvflg = 0;
static volatile uint8_t buff;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
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
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  // Helpful vars
  uint32_t len = 0; 						// Message length for UART
  char rcv[100] = ""; 						// Received characters buffer
  char msg[100] = "";						// Message to transmit over UART
  char frown[17] = "Invalid command.";		// Error message
  char dutychar[3] = ""; 					// Store slice of command containing duty
  char * dutychk1; 							// Ptr for checking duty char within 0-F
  char * dutychk2;							// Same as above for second digit
  char hexvalid[17] = "0123456789ABCDEF";	// List of valid duty chars
  char motchar[2] = "";						// Store slice of command containing motor select
  int8_t commduty; 							// Parse integer duty from motchar

  // Start timers
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

  // Make motors and disable them
  Motor_Str Spool_Mot =  {.duty           = 0,
                          .enable         = 0,
                          .timer          = &htim1,
                          .ChannelPair    = 1};
  motor_disable(&Spool_Mot);
  Motor_Str Table_Mot =  {.duty           = 0,
                          .enable         = 0,
                          .timer          = &htim1,
                          .ChannelPair    = 2};
  motor_disable(&Table_Mot);

  // Print to the console so we can see what's happening
  HAL_Delay(50);
  len = sprintf(&msg,"Powering on. Motors disabled.\r\nTimer 1 compare registers:\r\nCCR1: %ld\r\nCCR2: %ld\r\nCCR3: %ld\r\nCCR4: %ld\r\n", htim1.Instance->CCR1, htim1.Instance->CCR2, htim1.Instance->CCR3, htim1.Instance->CCR4);
  HAL_UART_Transmit_IT(&huart2, &msg, len);
  HAL_Delay(200);
  memset(&msg, '\0', 100);

  // Enable motors
  motor_enable(&Spool_Mot);
  motor_enable(&Table_Mot);
  len = sprintf(&msg,"Motors enabled.\r\nTimer 1 compare registers:\r\nCCR1: %ld\r\nCCR2: %ld\r\nCCR3: %ld\r\nCCR4: %ld\r\n", htim1.Instance->CCR1, htim1.Instance->CCR2, htim1.Instance->CCR3, htim1.Instance->CCR4);
  HAL_UART_Transmit_IT(&huart2, &msg, len);
  HAL_Delay(200);
  memset(&msg, '\0', 100);

  // Set the duty cycle
  //motor_set_duty(&Spool_Mot, +10);
  //motor_set_duty(&Table_Mot, -20);
  //len = sprintf(&msg,"Contents of htim1:\r\nCCR1: %ld\r\nCCR2: %ld\r\nCCR3: %ld\r\nCCR4: %ld\r\n", htim1.Instance->CCR1, htim1.Instance->CCR2, htim1.Instance->CCR3, htim1.Instance->CCR4);

  // Prompt user to enter commands
  len = sprintf(&msg,"Commands enabled. Enter using format <Mnxy>\r\nn = 1 | 2 (motor sel)\r\nxy = 00-FF (0x duty cycle)\r\n\n");
  HAL_UART_Transmit_IT(&huart2, &msg, len);
  HAL_Delay(200);
  memset(&msg, '\0', 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	// Receive characters over UART
	HAL_UART_Receive_IT(&huart2, &buff, 1);
	// When a character comes in over UART
	if (rcvflg == 1){
		// Push the new character into rcv
		strcat(&rcv, &buff);
		// Get the length of rcv
		len = strlen(rcv);
		// Trigger on return key
		if (rcv[len-1] == '\r'){
			// Validate correct command format
			// Correct format
			if (rcv[0] == 'M' && len == 5){
				// Parse command string
				motchar[0] = rcv[1];
				dutychar[0] = rcv[2];
				dutychk1 = strchr(hexvalid, dutychar[0]);
				dutychar[1] = rcv[3];
				dutychk2 = strchr(hexvalid, dutychar[1]);
				commduty = strtol(&dutychar, NULL, 16);
				// Scale duty cycle
				if (commduty < 0){
					commduty = (int8_t) ((float)commduty / 128 * 100);
				} else {
					commduty = (int8_t) ((float)commduty / 127 * 100);
				}
				// Choose requested motor and validate duty
				if (motchar[0] == '1' && dutychk1 != NULL && dutychk2 != NULL){
					// Send duty cycle to spool motor
					motor_set_duty(&Spool_Mot, commduty);
					len = sprintf(&msg, "\nSetting spool motor (%s) to %d percent duty\r\n", motchar, commduty);
				} else if (motchar[0] == '2' && dutychk1 != NULL && dutychk2 != NULL){
					// Send duty cycle to table motor
					motor_set_duty(&Table_Mot, commduty);
					len = sprintf(&msg, "\nSetting table motor (%s) to %d percent duty\r\n", motchar, commduty);
				} else if (dutychk1 == NULL || dutychk2 == NULL){
					// Invalid duty cycle
					len = sprintf(&msg, "\n%s Valid duty cycle is a hex number 00-FF.\r\n", frown);
				} else {
					// Invalid motor number
					len = sprintf(&msg, "\n%s Valid motor args are 1 and 2.\r\n", frown);
				}
			// Wrong size input or didn't start with 'M'
			} else {
				// Set message to frown
				len = sprintf(&msg, "\n%s Did you backspace?\r\n", frown);
			}
			// Send command result message to terminal
			HAL_Delay(50);
			HAL_UART_Transmit_IT(&huart2, &msg, len);
			HAL_Delay(50);
			memset(&msg, '\0', 100);
			memset(&rcv, '\0', 100); // Clear the receive buffer!
		}
		// Lower the character received flag
		rcvflg = 0;
		// Update htim1 display in terminal
		len = sprintf(&msg,"Timer 1 compare registers:\r\nCCR1: %ld\r\nCCR2: %ld\r\nCCR3: %ld\r\nCCR4: %ld\r\n\n", htim1.Instance->CCR1, htim1.Instance->CCR2, htim1.Instance->CCR3, htim1.Instance->CCR4);
		HAL_UART_Transmit_IT(&huart2, &msg, len);
		HAL_Delay(50);
		memset(&msg, '\0', 100);
	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// Receive CB fxn
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	// This is required?
	UNUSED(huart);
	// Signal to main code that a character was received
	rcvflg = 1;
	// Parrot character to terminal
	HAL_UART_Transmit_IT(&huart2, &buff, 1);
}

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
