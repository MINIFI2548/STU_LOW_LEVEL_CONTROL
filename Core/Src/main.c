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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUF_SIZE 10
#define ADC_BUF_SIZE_INV 0.1f

#define ENCODER_PPR 2048 // PPR
#define ENCODER_RESOLUTION (ENCODER_PPR * 4)
#define ENCODER_UPDATE_PERIOD 0.001f //s

#define SPEED_CONVERSION_FACTOR ((2.0f * M_PI) / ((float)ENCODER_RESOLUTION * ENCODER_UPDATE_PERIOD))

#define ADC_TO_CURRENT_LOAD_RATIO 0.05f
#define ADC_CURRENT_LOAD_OFFSET 2048 // offset before convert

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

__IO uint32_t BspButtonState = BUTTON_RELEASED;

/* USER CODE BEGIN PV */
volatile uint16_t adc_buffer[ADC_BUF_SIZE];
float adc_AVG;
volatile int32_t current_encoder = 0;
volatile int32_t previous_encoder = 0;
volatile float omega = 0.0f; // rad/s
volatile float current_load = 0.0f; //Amp

float dutycycle = 0.5f;

Motor_TypeDef myMotor1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Current_Update();
void Velocity_Update();
void Motor_Control();
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
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
//  เเริ่มต้น ADC DMA โดยระบุความยาวเป็น ADC_BUF_SIZE
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUF_SIZE);
//  เริ่มต้นอ่าน Encoder
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
////  เริ่มต้น ADC พร้อมกับ DMA (adc_buffer ต้องถูกประกาศเป็น global variable)
//  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
//  เริ่มต้น Main Control Loop (ต้องเปิดหลังสุด เพื่อให้ระบบอื่นพร้อมก่อน)
  HAL_TIM_Base_Start_IT(&htim6);

  //  ตั้งค่ามอเตอร์ (ชี้ไปที่ TIM8, CH1, ขา PC7, และสมมติค่า ARR = 8499)
  Motor_Init(&myMotor1, &htim8, TIM_CHANNEL_1, GPIOC, GPIO_PIN_7, 8499);


  /* USER CODE END 2 */

  /* USER CODE BEGIN BSP */

  /* USER CODE END BSP */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
//  Motor_SetPWM(&myMotor1, dutycycle);
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM6){
//		TIM6 Working on 100kHz
		// 1. Current Update Loop (Fast Loop)
		Current_Update();

		// 2. Velocity Update Loop (Slow Loop - 1kHz)
		static uint8_t speed_loop_counter = 0;
		speed_loop_counter++;
		if (speed_loop_counter >= 100)
		{
			Velocity_Update();
			speed_loop_counter = 0;
		}
	}
}

void Current_Update(){
	uint32_t sum_raw_current = 0;
	// หาผลรวมจาก Buffer
	for (uint8_t i = 0; i < ADC_BUF_SIZE; i++)
	{
		sum_raw_current += adc_buffer[i];
	}

	// ใช้การคูณ (0.1f) แทนการหาร (/ 10) เพื่อลด CPU Clock Cycles
	adc_AVG = (float)sum_raw_current * ADC_BUF_SIZE_INV;

	// แปลงเป็นกระแส (Amp)
	current_load = (adc_AVG - ADC_CURRENT_LOAD_OFFSET) * ADC_TO_CURRENT_LOAD_RATIO;
}

void Velocity_Update(){
	// อ่านค่า Encoder ปัจจุบัน (Cast เป็น int16_t เพื่อจัดการ Overflow)
	    current_encoder = __HAL_TIM_GET_COUNTER(&htim3);

	    // หา Delta Pulse
	    int32_t delta_pulse = current_encoder - previous_encoder;

	    uint8_t MULTI_TURN_QEI_CNT = __HAL_TIM_GET_AUTORELOAD(&htim3);
	    if(delta_pulse > MULTI_TURN_QEI_CNT / 2){
	    	delta_pulse -= MULTI_TURN_QEI_CNT;
	    }else if(delta_pulse < -(MULTI_TURN_QEI_CNT /2)){
	    	delta_pulse += MULTI_TURN_QEI_CNT;
	    }

	    // คำนวณความเร็วเชิงมุม (rad/s) ด้วยค่าคงที่ที่คำนวณไว้แล้ว
	    omega = (float)delta_pulse * SPEED_CONVERSION_FACTOR;

	    // เก็บค่าไว้ใช้รอบถัดไป
	    previous_encoder = current_encoder;
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
