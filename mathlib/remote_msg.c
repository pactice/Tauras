#include "remote_msg.h"
#include "stdlib.h"
#include "string.h"


rc_info_t rc;

/**
  * @brief       handle received rc data
  * @param[out]  rc:   structure to save handled rc data
  * @param[in]   buff: the buff which saved raw rc data
  * @retval 
  */
void rc_callback_handler(rc_info_t *rc, uint8_t *buff)
{
	if(buff == NULL)
	{
		return;
	}
  rc->ch1 = (buff[0]      | buff[1]  << 8) & 0x07FF;
  rc->ch1 -= 1024;
  rc->ch2 = (buff[1] >> 3 | buff[2]  << 5) & 0x07FF;
  rc->ch2 -= 1024;
  rc->ch3 = (buff[2] >> 6 | buff[3]  << 2 | buff[4] << 10) & 0x07FF;
  rc->ch3 -= 1024;
  rc->ch4 = (buff[4] >> 1 | buff[5]  << 7) & 0x07FF;
  rc->ch4 -= 1024;
  rc->ch5=  (buff[16]     | buff[17] << 8) & 0x07FF;
	rc->ch5 -= 1024;
  rc->sw1 = ((buff[5] >> 4) & 0x000C) >> 2;
  rc->sw2 = (buff[5] >> 4) & 0x0003;
#if MATCH_MODE
  if ((abs(rc->ch1) < 660) && \
      (abs(rc->ch2) < 660) && \
      (abs(rc->ch3) < 660) && \
      (abs(rc->ch4) < 660))
  {
    HAL_IWDG_Refresh(&hiwdg);
  }
#else 
  if ((abs(rc->ch1) > 660) || \
      (abs(rc->ch2) > 660) || \
      (abs(rc->ch3) > 660) || \
      (abs(rc->ch4) > 660))
  {
    memset(rc, 0, sizeof(rc_info_t));
  }
#endif	
	rc->mouse.x = buff[6] | (buff[7] << 8); // x axis
  rc->mouse.y = buff[8] | (buff[9] << 8);
  rc->mouse.z = buff[10] | (buff[11] << 8);
	
  rc->mouse.l = buff[12];
  rc->mouse.r = buff[13];

  rc->kb.key_code = buff[14] | buff[15] << 8; // key borad code

}

/**
  * @brief      单击校验函数
  * @param[in]  按键
  * @retval     0为非首次按下
	* @note				调用一次后按键失活
  */
int oneclick_verify(button_bit bit)
{
	static uint32_t now_click;
	static int oneclick_inf;
	static uint32_t one_click_flag=0;
	
	oneclick_inf = 0;
	now_click= rc.mouse.l<<16 | rc.mouse.r<<17 | rc.kb.key_code;
	
	if((~now_click) & 1<<bit)
	{
		one_click_flag |= (1<<bit);
	}else if(one_click_flag&(1<<bit))
	{
		oneclick_inf=(one_click_flag & (1 <<bit));
		one_click_flag &= ~(0x01 <<bit);//失能按键
	}
	return oneclick_inf;
}
