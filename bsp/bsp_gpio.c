#include "bsp_gpio.h"


uint8_t state_ITR = 0x00;

/**
 * 描述 板间io口通讯的初始化
 * @date 2022-03-29
 * @param {void}
 * @returns {void}
 */
void User_Gpio_Init (void)
{
    IO_SIG1_RESET;
    IO_SIG2_RESET;
    IO_SIG3_RESET;
}

/**
 * 描述 拉高气阀板的三个引脚
 * @date 2022-03-29
 * @param {void}
 * @returns {void}
 */
void Send_ResetSIG(void)
{
    IO_SIG1_SET;
    IO_SIG2_SET;
    IO_SIG3_SET;
}

/**
 * 描述 常闭型光电开关的检测    有障碍物则高电平
 * @date 2022-03-29
 * @param {void}
 * @returns {ITR_e}
 */
//ITR_e ITR_State_Get (void)
//{
//    switch (ITR1_GET)       //内有return，故不加break 
//    {
//        case GPIO_PIN_SET:
//        {
//            return EXIST;
//        }
//        case GPIO_PIN_RESET:
//        {
//            return NonEXIST;
//        }
//        default :
//            return UNKONWN;
//    }
//}

//ITR_e Block_Ensure (void)
//{
//    state_ITR = state_ITR << 4;
//	if( ITR_State_Get() == EXIST)
//	{
//		state_ITR = state_ITR | 0x01;
//	}
//    if( state_ITR == 0x11)
//    return EXIST;
//    else
//    return NonEXIST;
//}
