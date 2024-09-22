/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include <stdio.h>
#include <string.h>
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId LowPriorityHandle;
osThreadId MidPriorityHandle;
osThreadId HighPriorityHandle;
osMutexId MuxSemHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void LowPriorityTask(void const * argument);
void MidPriorityTask(void const * argument);
void HighPriorityTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of MuxSem */
  osMutexDef(MuxSem);
  MuxSemHandle = osMutexCreate(osMutex(MuxSem));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of LowPriority */
  osThreadDef(LowPriority, LowPriorityTask, osPriorityIdle, 0, 128);
  LowPriorityHandle = osThreadCreate(osThread(LowPriority), NULL);

  /* definition and creation of MidPriority */
  osThreadDef(MidPriority, MidPriorityTask, osPriorityIdle, 0, 128);
  MidPriorityHandle = osThreadCreate(osThread(MidPriority), NULL);

  /* definition and creation of HighPriority */
  osThreadDef(HighPriority, HighPriorityTask, osPriorityIdle, 0, 128);
  HighPriorityHandle = osThreadCreate(osThread(HighPriority), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN 4 */
/**
  * @brief 重定向c库函数printf到USARTx
  * @retval None
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart7, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
 
/**
  * @brief 重定向c库函数getchar,scanf到USARTx
  * @retval None
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart7, &ch, 1, 0xffff);
  return ch;
}
/* USER CODE END 4 */


/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_LowPriorityTask */
/**
* @brief Function implementing the LowPriority thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LowPriorityTask */
void LowPriorityTask(void const * argument)
{
  /* USER CODE BEGIN LowPriorityTask */
    static uint32_t i;
	osStatus xReturn;
  /* Infinite loop */
  for(;;)
  {
	  printf("LowPriority_Task Get Mutex\n");
	  // 获取互斥量，没获取到就一直等待
	  xReturn = osMutexWait(MuxSemHandle, osWaitForever);
	  if(osOK == xReturn)
	  {
		  printf("LowPriority_Task Running\n\n");
	  }
	  for(i=0; i<2000000; i++)
	  {
		  taskYIELD(); // 发起任务调度
	  }
	  printf("LowPriority_Task Release Mutex\r\n");
	  xReturn = osMutexRelease(MuxSemHandle); // 给出互斥量
	  osDelay(1000);
  }
  /* USER CODE END LowPriorityTask */
}

/* USER CODE BEGIN Header_MidPriorityTask */
/**
* @brief Function implementing the MidPriority thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MidPriorityTask */
void MidPriorityTask(void const * argument)
{
  /* USER CODE BEGIN MidPriorityTask */
  /* Infinite loop */
  for(;;)
  {
	  printf("MidPriority_Task Running\n");
	  osDelay(1000);
  }
  /* USER CODE END MidPriorityTask */
}

/* USER CODE BEGIN Header_HighPriorityTask */
/**
* @brief Function implementing the HighPriority thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_HighPriorityTask */
void HighPriorityTask(void const * argument)
{
  /* USER CODE BEGIN HighPriorityTask */
	osStatus xReturn;
  /* Infinite loop */
  for(;;)
  {
	  printf("HighPriority_Task Get Mutex\n");
	  xReturn = osMutexWait(MuxSemHandle, osWaitForever);
	  if(osOK == xReturn)
	  {
		  printf("HighPriority_Task Running\n");
	  }
	  printf("HighPriority_Task Release Mutex\r\n");
	  xReturn = osMutexRelease(MuxSemHandle);
	  osDelay(1000);
  }
  /* USER CODE END HighPriorityTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
