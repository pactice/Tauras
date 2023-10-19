/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "modeswitch_task.h"
#include "uplift_task.h"
#include "stretch_task.h"
#include "addstretch_task.h"
#include "rotate_task.h"
#include "convey_task.h"
#include "spin_task.h"
#include "cross_task.h"
#include "common_task.h"
#include "cradle_task.h"
#include "relay_task.h"
#include "bsp_can.h"
#include "bsp_judgesend.h"
#include "diy_remote.h"
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
osTimerId Can1SendTaskHandle;
osTimerId Can2SendTaskHandle;

osThreadId upliftTaskHandle;
osThreadId crossTaskHandle;
osThreadId cradleTaskHandle;
osThreadId stretchTaskHandle;
osThreadId addstretchTaskHandle;
osThreadId rotateTaskHandle;
osThreadId chassisTaskHandle;
osThreadId conveyTaskHandle;
osThreadId spinTaskHandle;
osThreadId relayTaskHandle;
osThreadId modeswitchTaskHandle;
osThreadId judgeSendTaskHandle;
osThreadId diyrefreshTaskHandle;

osMessageQId can1msqHandle;
osMessageQId can2msqHandle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

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

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

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
	osTimerDef(Can1Timer,Can1_Send_Task);
  Can1SendTaskHandle = osTimerCreate(osTimer(Can1Timer),osTimerPeriodic,NULL);
  osTimerStart(Can1SendTaskHandle,1);

  osTimerDef(Can2Timer,Can2_Send_Task);
  Can2SendTaskHandle = osTimerCreate(osTimer(Can2Timer),osTimerPeriodic,NULL);
  osTimerStart(Can2SendTaskHandle,1);
	
	
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
	 osMessageQDef(Can1msq,10,Current_Queue_t*);
  can1msqHandle = osMessageCreate(osMessageQ(Can1msq),NULL);

  osMessageQDef(Can2msq,10,Current_Queue_t *);
  can2msqHandle = osMessageCreate(osMessageQ(Can2msq),NULL);

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	//模式选择任务
	osThreadDef(modeswitchTask, modeswitch_task, osPriorityAboveNormal, 0, 512);
  modeswitchTaskHandle = osThreadCreate(osThread(modeswitchTask), NULL);
	//抬升
	osThreadDef(upliftTask, uplift_task, osPriorityAboveNormal, 0, 128);
  upliftTaskHandle = osThreadCreate(osThread(upliftTask), NULL);
	//前移
	osThreadDef(stretchTask, stretch_task, osPriorityAboveNormal, 0, 216);
  stretchTaskHandle = osThreadCreate(osThread(stretchTask), NULL);
	//前伸
	osThreadDef(addstretchTask, addstretch_task, osPriorityAboveNormal, 0, 128);
  stretchTaskHandle = osThreadCreate(osThread(addstretchTask), NULL);
	//滚动
	osThreadDef(rotateTask, rotate_task, osPriorityAboveNormal, 0, 128);
  rotateTaskHandle = osThreadCreate(osThread(rotateTask), NULL);
	//前yaw轴
	osThreadDef(crossTask, cross_task, osPriorityAboveNormal, 0, 128);
  crossTaskHandle = osThreadCreate(osThread(crossTask), NULL);
	//吸盘spin
	osThreadDef(spinTask, spin_task, osPriorityAboveNormal, 0, 128);
  spinTaskHandle = osThreadCreate(osThread(spinTask), NULL);
	//矿仓
	osThreadDef(conveyTask, convey_task, osPriorityAboveNormal, 0, 128);
  conveyTaskHandle = osThreadCreate(osThread(conveyTask), NULL);

	//数据传输
	osThreadDef(relayTask, relay_task, osPriorityAboveNormal, 0 ,128);
  relayTaskHandle = osThreadCreate(osThread(relayTask), NULL);
	//底盘
	osThreadDef(chassisTask, chassis_task, osPriorityAboveNormal, 0, 128);
  chassisTaskHandle = osThreadCreate(osThread(chassisTask), NULL);
	//云台
	osThreadDef(cradleTask, cradle_task, osPriorityAboveNormal, 0, 128);
  cradleTaskHandle = osThreadCreate(osThread(cradleTask), NULL);

	//裁判系统
	osThreadDef(judgesendTask, judge_send_task, osPriorityAboveNormal, 0, 128);
	judgeSendTaskHandle = osThreadCreate(osThread(judgesendTask),NULL);
	//自定义遥控器数据更新
//	osThreadDef(diyrefreshTask,diy_refresh_task,osPriorityAboveNormal,0,128);
//	diyrefreshTaskHandle = osThreadCreate(osThread(diyrefreshTask),NULL);
  /* USER CODE END RTOS_THREADS */

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

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
