#define __RELAY_TASK_GLOBALS
#include "relay_task.h"
#include "bsp_usart.h"
#include "cmsis_os.h"
#include "string.h"

#define RELAY_TASK_PERIOD 8

uint16_t lenth = 0;
uint8_t main_sendbuffer[29] = {0};//发送缓存
void relay_task(void const *argu)
{
    uint32_t mode_wake_time = osKernelSysTick();
//    static uint8_t main_sendbuffer[32] = {0};//发送缓存
    for(;;)
    {
        taskENTER_CRITICAL();
		main_send.frameheader = 0xAA;
		main_send.frametail = 0xBB;
		lenth = sizeof (main_send);
				main_send.chassis.vx = 1;
		main_send.chassis.vy = 2;
		main_send.chassis.vw = 3;
    main_send.chassis.angle_del = 4;
		main_send.chassis.ctrl_mode = 5;
			
//		main_send.chassis.vx = chassis.vx;
//		main_send.chassis.vy = chassis.vy;
//		main_send.chassis.vw = chassis.vw;
//    main_send.chassis.angle_del = chassis.angle_del;
//		main_send.chassis.ctrl_mode = chassis.ctrl_mode;
		lenth	= sizeof(main_send); 
		memcpy(main_sendbuffer,(uint8_t*)&main_send,sizeof(main_send));
		HAL_UART_Transmit_DMA(&RELAY_HUART, (uint8_t *)&main_sendbuffer,sizeof(main_sendbuffer));
    taskEXIT_CRITICAL();
    osDelayUntil(&mode_wake_time,RELAY_TASK_PERIOD);
    }

}
