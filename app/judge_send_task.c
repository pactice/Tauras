#include "bsp_judge.h"
#include "bsp_judgesend.h"
#include "bsp_usart.h"
#include "cmsis_os.h"
#include "keyboard_handle.h"
#include "modeswitch_task.h"
#include "relay_task.h"
#include "string.h"

extern ext_game_state_t					Game_State;

graphic_data_struct_t background[5];
uint8_t background_frame[3] = "bfa";
uint8_t separate1[3] 	= "sp1";
uint8_t separate2[3] 	= "sp2";
uint8_t separate3[3] 	= "sp3";
uint8_t engineer_mod_uid[3]="fun";
uint8_t engineer_mod_ui[3]="mod";
uint8_t engineer_help_ui[3]="ait";
uint8_t engineer_help_uid[3]="aid";
uint8_t engineer_heal_ui[3]="hel";
uint8_t engineer_heal_uid[3]="hed";
uint8_t engineer_heal_mode_ui[3]="hem";
uint8_t	ui_fun[30]  = "MODE:";
uint8_t	ui_assist[30] = "ASSIST:";
uint8_t	ui_heal[30] = "HEAL:";
uint8_t	ui_fun_detail[30]  = "MOVE";
uint8_t	ui_assist_detail[30] = "UNKNOWN";
uint8_t	ui_heal_detail[30] = "UNKNOWN";
uint8_t	ui_heal_mode[30] = "0";

graphic_data_struct_t	figure_line[5];
uint8_t figure_line0[3] = "LI1";
uint8_t figure_line1[3] = "LI2";
uint8_t figure_line2[3] = "LI3";
uint8_t figure_line3[3] = "LI4";
uint8_t figure_line4[3] = "LI5";

assist_action_e assist1;


static func_mode_e function_last;
static block_store_e block_last[3];

void judge_send_task(void const *argu)
{
	for(;;)
	{
		if(Game_State.game_progress == 1)
		{
			ui_init();//准备阶段ui初始化
		}else if(Game_State.game_progress)
		{
			ui_mode_display(func_mode);
			ui_block_display(block_state);
			//ui_help_display(main_send.assist_action);
			
			ui_tips_display();
		}else if(!Game_State.game_progress)//无比赛
		{
			ui_init();//ui初始化	
		}
	osDelay(JUDGESEND_PERIOD);
	}
}

/**
	* @brief  自定义ui初始化
  * @param  void
  * @retval void
	* @attention  初始图形发送
  */
void ui_init(void)
{
	judge_get_basedata();

	static int ui_init_flag = 1; 
	if(ui_init_flag)
	{
		for(int i = 0; i < JUDGESEND_TIME; i++)
		{
			ui_delete(type_delete_all,0);
			osDelay(JUDGESEND_PERIOD);
		}	
		ui_init_flag=0;
	}
	
	judge_get_basedata();

	//图层2		模式内容
	ui_sendcode(engineer_mod_ui,1,2,0,20,2,50,875,4,ui_fun);			//MODE:
	osDelay(JUDGESEND_PERIOD);
	ui_sendcode(engineer_mod_uid,1,2,0,20,2,140,875,4,ui_fun_detail);	//模式具体模式
	osDelay(JUDGESEND_PERIOD);
//	ui_sendcode(engineer_help_ui,1,2,0,20,2,50,830,4,ui_assist);		//ASSIST:
//	osDelay(JUDGESEND_PERIOD);
//	ui_sendcode(engineer_help_uid,1,2,0,20,2,180,830,4,ui_assist_detail);//ASSIST具体内容
//	osDelay(JUDGESEND_PERIOD);
//	ui_sendcode(engineer_heal_ui,1,2,0,20,2,50,785,4,ui_heal);			//HEAL:
//	osDelay(JUDGESEND_PERIOD);
//	ui_sendcode(engineer_heal_uid,1,2,0,20,2,140,785,4,ui_heal_detail);	//HEAL具体内容
//	osDelay(JUDGESEND_PERIOD);
	//图层1		矿石对位
	graphic_data_struct_t	location_line[3];
	uint8_t under_line[3] = "udr";
	uint8_t right_line[3] = "rgt";
	uint8_t mid_line[3] = "mid";	
	ui_data_packed_line(&location_line[0],under_line,1,1,0,1,0,560,1920,560);
	ui_data_packed_line(&location_line[1],mid_line,1,1,0,1,920,0,920,1080);
//	ui_data_packed_line(&location_line[2],right_line,1,1,0,1,1110,0,1110,1080);
	ui_sendgraph(DRAW_IMAGINE2, (uint8_t*)location_line,CLIENT_DRAW_2_GRAPHS_CMD_ID);
	osDelay(JUDGESEND_PERIOD);
	//图层3		矿石状态
	ui_data_packed_line(&figure_line[0],figure_line0,1,3,8,5,50,706,190,706);		//横线
	ui_data_packed_line(&figure_line[1],figure_line1,0,3,1,40,150,730,190,730);		//此处的三个矿均为空操作
	ui_data_packed_line(&figure_line[2],figure_line2,0,3,1,40,100,730,140,730);
	ui_data_packed_line(&figure_line[3],figure_line3,0,3,1,40,50,730,90,730);
	ui_data_packed_line(&figure_line[4],figure_line4,1,3,8,5,145,750,145,710);		//竖线
	ui_sendgraph(DRAW_IMAGINE5, (uint8_t*)figure_line,CLIENT_DRAW_5_GRAPHS_CMD_ID);
	osDelay(JUDGESEND_PERIOD);
}

//功能函数
void ui_mode_display(func_mode_e function)
{

	if(function != function_last)
	{
		for(int i = 0; i < JUDGESEND_TIME; i++)
		{
			switch(function)
			{
				case(MOVE_MODE):
				{
					uint8_t	func_move[30] = "MOVE";
					ui_sendcode(engineer_mod_uid,2,2,0,20,2,140,875,4,func_move);
				}break;

				case(GET_GOLD_MODE):
				{
					uint8_t	func_getgold[30] = "GOLD";
					ui_sendcode(engineer_mod_uid,2,2,0,20,2,140,875,4,func_getgold);
				}break;
				case(ONELICK_AIRGET):
				{
					uint8_t	func_onclick[30] = "ONELICK_AIRGET";
					ui_sendcode(engineer_mod_uid,2,2,0,20,2,140,875,4,func_onclick);
				}
				case(PICK_BLOCK_MODE):
				{
					uint8_t	func_pickblock[30] = "PICK";
					ui_sendcode(engineer_mod_uid,2,2,0,20,2,140,875,4,func_pickblock);
				}break;

				case(EXCHANGE_MODE):
				{
					uint8_t	func_exchangeblock[30] = "EXCHG";
					ui_sendcode(engineer_mod_uid,2,2,0,20,2,140,875,5,func_exchangeblock);			
				}break;
				case(REMOTE_GET):
				{
				
				}break;
			}
			function_last = function;
			osDelay(JUDGESEND_PERIOD);
		}
	}
}

void ui_block_display(block_store_e * ui_block_state)
{

	for(int i=0;i<3;i++)
	{
		if(ui_block_state[i]!=block_last[i])
		{
			for(int j = 0; j < JUDGESEND_TIME; j++)
			{
				if(ui_block_state[i])
				{
					switch (i)
					{
						case(0):
						{
							ui_data_packed_line(&figure_line[1],figure_line1,1,3,1,40,150,730,190,730);
							ui_sendgraph(DRAW_IMAGINE5, (uint8_t*)figure_line,CLIENT_DRAW_5_GRAPHS_CMD_ID);
						}break;

						case(1):
						{
							ui_data_packed_line(&figure_line[2],figure_line2,1,3,1,40,100,730,140,730);
							ui_sendgraph(DRAW_IMAGINE5, (uint8_t*)figure_line,CLIENT_DRAW_5_GRAPHS_CMD_ID);
						}break;

						case(2):
						{
							ui_data_packed_line(&figure_line[3],figure_line3,1,3,1,40,50,730,90,730);
							ui_sendgraph(DRAW_IMAGINE5, (uint8_t*)figure_line,CLIENT_DRAW_5_GRAPHS_CMD_ID);
						}break;				
					}
					osDelay(JUDGESEND_PERIOD);
				}else if(!ui_block_state[i])
				{
					switch (i)
					{
						case(0):
						{
							ui_data_packed_line(&figure_line[1],figure_line1,3,3,1,40,150,730,190,730);
							ui_sendgraph(DRAW_IMAGINE5, (uint8_t*)figure_line,CLIENT_DRAW_5_GRAPHS_CMD_ID);
						}break;

						case(1):
						{
							ui_data_packed_line(&figure_line[2],figure_line2,3,3,1,40,100,730,140,730);
							ui_sendgraph(DRAW_IMAGINE5, (uint8_t*)figure_line,CLIENT_DRAW_5_GRAPHS_CMD_ID);
						}break;

						case(2):
						{
							ui_data_packed_line(&figure_line[3],figure_line3,3,3,1,40,50,730,90,730);
							ui_sendgraph(DRAW_IMAGINE5, (uint8_t*)figure_line,CLIENT_DRAW_5_GRAPHS_CMD_ID);
						}break;				
					}
					osDelay(JUDGESEND_PERIOD);
				}			
			}
			block_last[i]=ui_block_state[i];
		}
	}
}

void ui_help_display(assist_action_e assist_state)
{
	static assist_action_e help_last;
	if(assist_state!=help_last)
	{
		for(int i = 0; i < JUDGESEND_TIME; i++)
		{
			if( assist_state == HELP_ON )
			{
				uint8_t	ui_assist_on[30] = "ASSIST_ON";
				ui_sendcode(engineer_help_uid,2,2,0,20,2,180,830,4,ui_assist_on);//ASSIST具体内容
			}else
			{
				uint8_t	ui_assist_off[30] = "ASSIST_OFF";
				ui_sendcode(engineer_help_uid,2,2,0,20,2,180,830,4,ui_assist_off);//ASSIST具体内容	
			}	
			osDelay(JUDGESEND_PERIOD);			
		}
		help_last = assist_state;
	}
	osDelay(JUDGESEND_PERIOD);
}

void ui_tips_display(void)
{
	uint8_t glod_tips[3] = "TIP";
	uint8_t	ui_glod_tips[30] = "GLOD COMING";
	if ((Game_State.game_progress == 4)&&(Game_State.stage_remain_time<260)&&(Game_State.stage_remain_time>240))
	{
		ui_sendcode(glod_tips,1,1,0,25,2,840,420,1,ui_glod_tips);
		osDelay(300);
		ui_sendcode(glod_tips,3,1,0,25,2,840,420,1,ui_glod_tips);
		osDelay(300);
	}
	else
	{
		ui_sendcode(glod_tips,3,1,0,25,2,840,420,1,ui_glod_tips);
		osDelay(JUDGESEND_PERIOD);
	}
	
}

//void ui_test (void)
//{
//	judge_get_basedata();			//获取裁判系统数据
//	graphic_data_struct_t	location_line[2];
//	uint8_t left_line[3] = "lft";
//	uint8_t mid_line[3]="mid";
//	uint8_t right_line[3] = "rgt";
//	ui_data_packed_line(&location_line[0],left_line,1,9,0,1,760,0,860,360);
//	ui_data_packed_line(&location_line[0],mid_line,1,9,0,1,760,0,860,360);
//	ui_data_packed_line(&location_line[1],right_line,1,9,0,1,1160,0,1060,360);
//	ui_sendgraph(DRAW_IMAGINE2, (uint8_t*)location_line,CLIENT_DRAW_2_GRAPHS_CMD_ID);
//	osDelay(20);
//}
/************************模式提示*********************/
//ui_sendcode(engineer_mod_ui,1,2,0,20,2,50,875,4,test1);			//MODE
//ui_sendcode(engineer_func_ui,2,2,0,20,2,140,875,4,test3);			//模式具体模式
//ui_sendcode(engineer_assist_ui,1,2,0,20,2,50,830,4,test3);		//ASSIST
//ui_sendcode(engineer_heal_ui,1,2,0,20,2,180,830,4,test4);			//ASSIST具体内容
//ui_sendcode(engineer_heal_ui,1,2,0,20,2,50,785,4,test5);			//HEAL
//ui_sendcode(engineer_heal_ui,1,2,0,20,2,140,785,4,test6);			//HEAL具体内容

/************************金矿提示*********************/
//uint8_t glod_tips[3] = "TIP";
//uint8_t	ui_heal_mode[30] = "GLOD COMING";
//ui_sendcode(glod_tips,1,1,0,25,2,840,320,1,ui_heal_mode);
//osDelay(300);
//ui_sendcode(glod_tips,3,1,0,25,2,840,320,1,ui_heal_mode);
//osDelay(300);
/***********************矿石状态***********************/
//graphic_data_struct_t	figure_line[5];
//uint8_t figure_line0[3] = "LI1";
//uint8_t figure_line1[3] = "LI2";
//uint8_t figure_line2[3] = "LI3";
//uint8_t figure_line3[3] = "LI4";
//uint8_t figure_line4[3] = "LI5";
//ui_data_packed_line(&figure_line[0],figure_line0,1,9,8,5,50,706,190,706);			横线
//ui_data_packed_line(&figure_line[1],figure_line1,1,9,1,40,150,730,190,730);
//ui_data_packed_line(&figure_line[2],figure_line2,1,9,1,40,100,730,140,730);
//ui_data_packed_line(&figure_line[3],figure_line3,1,9,1,40,50,730,90,730);
//ui_data_packed_line(&figure_line[4],figure_line4,1,9,8,5,145,750,145,710);
//ui_sendgraph(DRAW_IMAGINE5, (uint8_t*)figure_line,CLIENT_DRAW_5_GRAPHS_CMD_ID);	竖线
/***********************矿石对位************************/
//graphic_data_struct_t	location_line[2];
//uint8_t left_line[3] = "lft";
//uint8_t right_line[3] = "rgt";
//ui_data_packed_line(&location_line[0],left_line,1,9,0,1,760,0,860,360);
//ui_data_packed_line(&location_line[1],right_line,1,9,0,1,1160,0,1060,360);
//ui_sendgraph(DRAW_IMAGINE2, (uint8_t*)location_line,CLIENT_DRAW_2_GRAPHS_CMD_ID);
