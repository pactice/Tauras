#include "common_task.h"
#include "cmsis_os.h"
#include "bsp_can.h"


extern osMessageQId can1msqHandle;
extern osMessageQId can2msqHandle;


void Can1_Send_Task(const void *argument )
{
    taskENTER_CRITICAL();
    
    osEvent evt;
    
    static uint8_t ID_flag  = 0;
    Current_Queue_t *msgq = NULL;

    evt = osMessageGet(can1msqHandle,0);
    if(evt.status == osEventMessage) 	//判断是否成功收到消息
    {
        msgq = (Current_Queue_t *)evt.value.p;
        switch(ID_flag)
        {
            case 0:
                Canx_SendCurrent(&hcan1,0x200,msgq->Current_U1.SendCurrent);
                ID_flag = 1;
                break;
            case 1:
                Canx_SendCurrent(&hcan1,0x1FF,msgq->Current_U2.SendCurrent);
                ID_flag = 0;
                break;
            default:
                break;
        }
    } 
    taskEXIT_CRITICAL();
}


void Can2_Send_Task(const void *argument )
{
    taskENTER_CRITICAL();

    osEvent evt;
    static uint8_t ID_flag  = 0;
    Current_Queue_t *msgq = NULL;

     evt = osMessageGet(can2msqHandle,0);
    if(evt.status == osEventMessage) 	//判断是否成功收到消息
    {
        msgq = (Current_Queue_t *)evt.value.p;
        switch(ID_flag)
        {
            case 0:
                Canx_SendCurrent(&hcan2,0x200,msgq->Current_U1.SendCurrent);
                ID_flag = 1;
                break;
            case 1:
                Canx_SendCurrent(&hcan2,0x1FF,msgq->Current_U2.SendCurrent);
                ID_flag = 0;
                break;
            default:
                break;
        }
    }
    taskEXIT_CRITICAL();
}

