/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "fatfs.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "sd_card.h"
#include "stm32f4xx.h"  // Needed for SCB and __set_MSP
#include "app_handler.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define APP_SP_ADD (APP_START_ADDRESS)
#define APP_RESET_HANDLER   (APP_SP_ADD + 4)
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint32_t APP_START_ADDRESS;

FATFS fs;
FIL file;
FRESULT res;
UINT bw;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Test_FATFS_SPI(void)
{
    // Mount filesystem
    uint8_t ret = f_mount(&fs, "", 1);
    printf("ret - %d\r\n", ret);
    if (ret == FR_OK)
    {
        // Create and open file
        res = f_open(&file, "hello2.txt", FA_WRITE | FA_CREATE_ALWAYS);
        printf("res - %d\r\n", res);
        if (res == FR_OK)
        {
            char *text = "Hello from STM32 SPI FATFS!\r\n";
            f_write(&file, text, strlen(text), &bw);
            printf("write success\r\n");

            f_close(&file);
        }
        // Unmount
        f_mount(NULL, "", 1);
    }
    else
    {
        printf("Culd not mount\r\n");
    }
}

void jump_to_app();

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
    MX_USART1_UART_Init();
    MX_RTC_Init();
    MX_SPI1_Init();
    SD_Init(); // Or whatever your function name is
    MX_FATFS_Init();
    /* USER CODE BEGIN 2 */

    printf("*(uint32_t*) (APP_START_ADDRESS) = %ld\r\n", *(uint32_t*) (APP_START_ADDRESS));
    fflush(stdout);
    HAL_Delay(1000);

    if (*(uint32_t*) (APP_START_ADDRESS) == 0xFFFFFFFF)
    {
        printf("No Code Found\r\n");
//      Test_FATFS_SPI();

        start_firmware_update();
        if (*(int32_t*) (APP_START_ADDRESS) != -1)
        {
        printf("YES Code Found - %8lX %8lX\r\n", *(uint32_t*) (APP_START_ADDRESS), *(uint32_t*) (APP_START_ADDRESS + 4));
            jump_to_app();
        }
    }
    else
    {
        printf("YES Code Found - %8lX %8lX\r\n", *(uint32_t*) (APP_START_ADDRESS),
                        *(uint32_t*) (APP_START_ADDRESS + 4));
        jump_to_app();
    }
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
        HAL_Delay(100);
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
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 160;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
typedef void (*pFunction)(void);

void jump_to_app(void)
{
    uint32_t app_stack = *(volatile uint32_t*) APP_SP_ADD;

    uint32_t app_reset = *(volatile uint32_t*) APP_RESET_HANDLER;

    // Check if application is valid (optional but highly recommended)
//    if ((app_stack & 0x2FFE0000) == 0x20000000) // Valid RAM range
    {
        printf("If \r\n");
        // De-initialize HAL (optional, depends on your bootloader)
        HAL_DeInit();

        // Disable all interrupts
        __disable_irq();

        // Set vector table offset register to application's vector table
        SCB->VTOR = APP_SP_ADD;

        // Set MSP to application's stack pointer
        __set_MSP(app_stack);
        app_reset |= 0x1;
        // Jump to application's reset handler
        ((pFunction) app_reset)();
    }
}

//int _write(int file, char *ptr, int len)
//{
//    HAL_UART_Transmit(&huart1, (uint8_t*) ptr, len, 100);
//    return len;
//}
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
