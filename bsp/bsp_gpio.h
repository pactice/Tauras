#ifndef __BSP_GPIO__

#define __BSP_GPIO__

#include "stm32f407xx.h"
#include "main.h"

typedef enum
{
    UNKONWN,
    EXIST,
    NonEXIST,
}ITR_e;


#define IO_SIG1_SET HAL_GPIO_WritePin(SIG_1_GPIO_Port , SIG_1_Pin ,GPIO_PIN_SET)
#define IO_SIG2_SET HAL_GPIO_WritePin(SIG_2_GPIO_Port , SIG_2_Pin ,GPIO_PIN_SET)
#define IO_SIG3_SET HAL_GPIO_WritePin(SIG_3_GPIO_Port , SIG_3_Pin ,GPIO_PIN_SET)
#define IO_SIG1_RESET HAL_GPIO_WritePin(SIG_1_GPIO_Port , SIG_1_Pin ,GPIO_PIN_RESET)
#define IO_SIG2_RESET HAL_GPIO_WritePin(SIG_2_GPIO_Port , SIG_2_Pin ,GPIO_PIN_RESET)
#define IO_SIG3_RESET HAL_GPIO_WritePin(SIG_3_GPIO_Port , SIG_3_Pin ,GPIO_PIN_RESET)

#define ITR1_GET HAL_GPIO_ReadPin(ITR_PIN1_GPIO_Port,ITR_PIN1_Pin)
#define ITR2_GET HAL_GPIO_ReadPin(ITR_PIN2_GPIO_Port,ITR_PIN2_Pin)

void User_Gpio_Init (void);
void Send_ResetSIG(void);
ITR_e ITR_State_Get (void);
ITR_e Block_Ensure (void);

#endif
