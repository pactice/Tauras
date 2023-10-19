//Channel：选择通道（1-10）
//传入单精度浮点数
//在函数的最下方的DataWave函数修改发送的数据
//其他函数无需修改
#include "DataScope_DP.h"
#include "usart.h"
#include "rotate_task.h"
#include "bsp_can.h"
#include "pid.h"
#define ABS(x)		((x>0)? (x): (-x))

typedef struct
{
    unsigned char DataScope_OutPut_Buffer[44];	   //串口发送缓冲区
    unsigned char Send_Count; //串口需要发送的数据个数
    unsigned char DataCnt;          //计数变量
} DataTypedfef;

DataTypedfef CK;	//传输数据用到的结构体

//函数说明：将单精度浮点数据转成4字节数据并存入指定地址
//target:目标单精度数据
//buf:待写入数组
//beg:指定从数组第几个元素开始写入
void Float2Byte(float *target,unsigned char *buf,unsigned char beg)
{
    unsigned char *point;
    point = (unsigned char*)target;	  //得到float的地址
    buf[beg]   = point[0];
    buf[beg+1] = point[1];
    buf[beg+2] = point[2];
    buf[beg+3] = point[3];
}


//函数说明：将待发送通道的单精度浮点数据写入发送缓冲区
//Data：通道数据
//Channel：选择通道（1-10）
void DataScope_Get_Channel_Data(float Data,unsigned char Channel)
{
    if ( (Channel > 10) || (Channel == 0) ) 
        return;  //通道个数大于10或等于0，直接跳出，不执行函数
    else
    {
    #ifdef MINIBALANCE
        Float2Byte(&Data,CK.DataScope_OutPut_Buffer,( (Channel-1) * 4 +1 ) );  //留出帧头
    #else  //VOFA
        Float2Byte(&Data,CK.DataScope_OutPut_Buffer,( (Channel-1) * 4) );
    #endif
    }
}


//函数说明：生成能正确识别的帧格式
//Channel_Number，需要发送的通道个数
//返回发送缓冲区数据个数
unsigned char DataScope_Data_Generate(unsigned char Channel_Number)
{
    if ( (Channel_Number > 10) || (Channel_Number == 0) )
    {
        return 0;    //通道个数大于10或等于0，直接跳出，不执行函数
    }
    else
    {
    #ifdef MINIBALANCE
        CK.DataScope_OutPut_Buffer[0] = '$';  //帧头
        uint8_t temp_cnt = Channel_Number*4+1;
        CK.DataScope_OutPut_Buffer[temp_cnt]  =  temp_cnt;  //帧尾
        return (temp_cnt+1);  //返回一个数据包的字节数
    #else  //VOFA+
        uint8_t temp_cnt = Channel_Number*4+4;
        CK.DataScope_OutPut_Buffer[4*Channel_Number + 0] = 0x00;
        CK.DataScope_OutPut_Buffer[4*Channel_Number + 1] = 0x00;
        CK.DataScope_OutPut_Buffer[4*Channel_Number + 2] = 0x80;
        CK.DataScope_OutPut_Buffer[4*Channel_Number + 3] = 0x7f;
        return temp_cnt;  //返回一个数据包的字节数
    #endif
    }
}

//函数说明：上位机通过串口打印数据波形
//附加说明：直接在主函数中调用此函数
void DataWave(UART_HandleTypeDef* huart)
{
    //Channel：选择通道（1-10）
    //传入单精度浮点数
    {
//    DataScope_Get_Channel_Data((float) rotate.cnt_ref, 1 );
    DataScope_Get_Channel_Data((float) rotate.ramp_set, 2 );
    DataScope_Get_Channel_Data((float) rotate.cnt_fdb[1], 3 );
//    DataScope_Get_Channel_Data((float) moto_rotate[1].given_current, 1 );
    DataScope_Get_Channel_Data((float) rotate.spd_ref[1], 4 );
//    DataScope_Get_Channel_Data((float) rotate.current[1], 4 );
    DataScope_Get_Channel_Data((float) moto_rotate[1].speed_rpm, 1 );
    DataScope_Get_Channel_Data((float) pid_rotate_stay_spd[1].pos_out, 5 );
    }
    CK.Send_Count = DataScope_Data_Generate(5);//串口需要发送的数据个数

    for( CK.DataCnt = 0 ; CK.DataCnt < CK.Send_Count; CK.DataCnt++)
    {
        while((huart->Instance->SR&0X40)==0);
        huart->Instance->DR = CK.DataScope_OutPut_Buffer[CK.DataCnt];
    }
}

