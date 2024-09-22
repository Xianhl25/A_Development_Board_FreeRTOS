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
#include "gpio.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
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
osThreadId ReceiveHandle;
osThreadId SendHandle;
osMessageQId TestQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void ReceiveTask(void const * argument);
void SendTask(void const * argument);

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

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of TestQueue */
  osMessageQDef(TestQueue, 16, uint32_t);
  TestQueueHandle = osMessageCreate(osMessageQ(TestQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Receive */
  osThreadDef(Receive, ReceiveTask, osPriorityIdle, 0, 128);
  ReceiveHandle = osThreadCreate(osThread(Receive), NULL);

  /* definition and creation of Send */
  osThreadDef(Send, SendTask, osPriorityIdle, 0, 128);
  SendHandle = osThreadCreate(osThread(Send), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

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

/* USER CODE BEGIN Header_ReceiveTask */
/**
* @brief Function implementing the Receive thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ReceiveTask */
void ReceiveTask(void const * argument)
{
  /* USER CODE BEGIN ReceiveTask */
	osEvent event;
  /* Infinite loop */
  for(;;)
  {
    event = osMessageGet(TestQueueHandle, osWaitForever); /* 用于从一个队列中接收消息并把消息从队列中删除 */ 
	if(osEventMessage == event.status)
	{
		printf("receive data: %d\n\n", event.value.v);
	}
	else
	{
		printf("error: 0x%d\n", event.status);
	}
  }
  /* USER CODE END ReceiveTask */
}

/* USER CODE BEGIN Header_SendTask */
/**
* @brief Function implementing the Send thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SendTask */
void SendTask(void const * argument)
{
  /* USER CODE BEGIN SendTask */
  osEvent xReturn;
  uint32_t send_data1 = 1;
  /* Infinite loop */
  for(;;)
  {
//	printf("send_data1\n");
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_SET)
	{
		/* KEY被按下 */
		printf("send_data1!\n");
		xReturn.status = osMessagePut(TestQueueHandle, send_data1, 0); /* 向队列尾部发送一个队列消息，消息以拷贝的形式入队 */
		if(osOK != xReturn.status)
		{
			printf("send fail!\n\n");
		}
	}
	osDelay(100);
  }
  /* USER CODE END SendTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
