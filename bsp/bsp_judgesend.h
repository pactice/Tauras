#ifndef BSP_JUDGESEND_H
#define BSP_JUDGESEND_H

#include "bsp_judge.h"
#include "keyboard_handle.h"
#include "modeswitch_task.h"
#include "relay_task.h"
#include "stdint.h"

typedef enum 
{
	BLUE,
	RED
}color_e;

typedef __packed struct
{
	color_e	 color;				//��Ӫ
	uint16_t robot_ID;		//����id
	uint16_t client_ID;		//��Ӧ�ͻ���id
	uint16_t self_HP;			//����Ѫ��
}self_inf_t;

typedef enum 
{
	DRAW_IMAGINE1=30,
	DRAW_IMAGINE2=45,
	DRAW_IMAGINE5=90,
	DRAW_IMAGINE7=120,
	PRINTF_CODE	=60,
	JUDGE_DELETE=17,
	ROBOT_COMM=(15+LEN_robot_interract),
}Judgesend_length_e;//�Զ������ݷ����ܳ���


/* ����ϵͳ����֡֡ͷ+�������ݽ�����Ϣ֡ͷ */
typedef __packed struct
{
	frame_header   									txFrameHeader;//֡ͷ5
	cmd_ID		 											CmdID;//������2
}SendClient_header_t;

typedef enum
{
	CLIENT_INTERACTIVE_CMD_ID_TEST    = 0x0200 , //�������ݣ�����0x0200~0x02ffѡȡ������ID�����ɲ������Զ���
	CLIENT_DELETE_GRAPH_CMD_ID     		= 0x0100 , //�ͻ���ɾ��ͼ��
	CLIENT_DRAW_1_GRAPHS_CMD_ID     	= 0x0101 , //�ͻ��˻���1��ͼ��
	CLIENT_DRAW_2_GRAPHS_CMD_ID    		= 0x0102 , //�ͻ��˻���2��ͼ��
	CLIENT_DRAW_5_GRAPHS_CMD_ID    		= 0x0103 , //�ͻ��˻���5��ͼ��
	CLIENT_DRAW_7_GRAPHS_CMD_ID    		= 0x0104 , //�ͻ��˻���7��ͼ��
	CLIENT_WRITE_STRINGS_CMD_ID    		= 0x0110 , //�ͻ��˻����ַ���ͼ��
}client_data_cmd_e;//���ݶε����� ID

/* �������ݽ�����Ϣ֡ͷ */
typedef __packed struct 
{ 
	client_data_cmd_e data_cmd_id;    
	uint16_t 					send_ID;    
	uint16_t 					receiver_ID; 
} judgesend_custom_header_t;

typedef __packed struct
{
	frame_header   							txFrameHeader;//֡ͷ5
	cmd_ID		 									CmdID;//������2;
	judgesend_custom_header_t		data_header;//���ݶ�֡ͷ 6
	//�˴�Ϊ�������ݶ�
	uint16_t		 								FrameTail;//֡β 2
}judgesend_frame_t;


/* ͼ��ɾ�� */
typedef enum
{
	type_delete_nop   = 0,
	type_delete_layer = 1,
	type_delete_all   = 2,
}type_graphic_delete_e;//ͼ��ɾ������

/*�ͻ���ɾ��ͼ��*/
typedef __packed struct
{ 
	type_graphic_delete_e operate_tpye;
	uint8_t layer;
} judgesend_delete_t;

/* ͼ������*/
typedef __packed struct 
{ 
	uint8_t graphic_name[3]; 
	uint16_t operate_tpye:3; 
	uint16_t graphic_tpye:3;
	uint16_t layer:4;
	uint16_t color:4; 
	uint16_t start_angle:9;
	uint16_t end_angle:9;
	uint16_t width:10;
	uint16_t start_x:11;
	uint16_t start_y:11;
	uint16_t radius:10; 
	uint16_t end_x:11;
	uint16_t end_y:11;
} graphic_data_struct_t;

/* ��������*/
typedef __packed struct 
{ 
	uint8_t graphic_name[3]; 
	uint16_t operate_tpye:3; 
	uint16_t graphic_tpye:3;
	uint16_t layer:4;
	uint16_t color:4; 
	uint16_t start_angle:9;
	uint16_t end_angle:9;
	uint16_t width:10;
	uint16_t start_x:11;
	uint16_t start_y:11;
	float num;
} float_data_struct_t;

/* ��������*/
typedef __packed struct 
{ 
	uint8_t graphic_name[3]; 
	uint16_t operate_tpye:3; 
	uint16_t graphic_tpye:3;
	uint16_t layer:4;
	uint16_t color:4; 
	uint16_t start_angle:9;
	uint16_t end_angle:9;
	uint16_t width:10;
	uint16_t start_x:11;
	uint16_t start_y:11;
	int32_t num;
} int_data_struct_t;

/* �����ַ� */
typedef __packed struct 
{
	graphic_data_struct_t grapic_data_struct; 
	uint8_t data[30];
} judgesend_code_t;

/*С��ͼ������Ϣ*/
typedef __packed struct
{
	float target_position_x;		//Ŀ��xλ�����꣬��λm		������Ŀ�������IDʱ������Ϊ0
	float target_position_y;		//Ŀ��yλ�����꣬��λm 		������Ŀ�������IDʱ������Ϊ0
	float target_position_z;		//Ŀ��zλ�����꣬��λm 		������Ŀ�������IDʱ������Ϊ0
	uint8_t commd_keyboard;			//����ָ��ʱ����̨�ְ��µļ�����Ϣ 		�ް���������Ϊ0
	uint16_t target_robot_ID;		//Ҫ���õ�Ŀ�������ID 		������λ����Ϣʱ������Ϊ0
}judgesend_map_t;

void judge_get_basedata(void);

void ui_data_packed_line(
			graphic_data_struct_t *line_data,
			uint8_t line_name[3],
			uint16_t operate_tpye,
			uint16_t layer,
			uint16_t color,
			uint16_t line_width,
			uint16_t line_startx,
			uint16_t line_starty,
			uint16_t line_endx,
			uint16_t line_endy);
void ui_data_packed_rectangle(
			graphic_data_struct_t *rect_data,
			uint8_t rect_name[3],
			uint16_t operate_tpye,
			uint16_t layer,
			uint16_t color,
			uint16_t rect_width,
			uint16_t rect_startx,
			uint16_t rect_starty,
			uint16_t rect_endx,
			uint16_t rect_endy);
void ui_data_packed_round (
			graphic_data_struct_t *round_data,
			uint8_t round_name[3],
			uint16_t operate_tpye,
			uint16_t layer,
			uint16_t color,
			uint16_t round_width,
			uint16_t round_centerx,
			uint16_t round_centery,
			uint16_t round_radius);
void ui_data_packed_oval (
			graphic_data_struct_t *oval_data,
			uint8_t oval_name[3],
			uint16_t operate_tpye,
			uint16_t layer,
			uint16_t color,
			uint16_t oval_width,
			uint16_t oval_centerx,
			uint16_t oval_centery,
			uint16_t oval_lengthx,
			uint16_t oval_lengthy);
void ui_data_packed_arc (
			graphic_data_struct_t *arc_data,
			uint8_t arc_name[3],
			uint16_t operate_tpye,
			uint16_t layer,
			uint16_t color,
			uint16_t arc_width,
			uint16_t arc_startangle,
			uint16_t arc_endangle,
			uint16_t arc_centerx,
			uint16_t arc_centery,
			uint16_t arc_lengthx,
			uint16_t arc_lengthy);
void ui_data_packed_float (
			float_data_struct_t *float_data,
			uint8_t float_name[3],
			uint16_t operate_tpye,
			uint16_t layer,
			uint16_t color,
			uint16_t float_size,
			uint16_t float_precision,
			uint16_t float_width,
			uint16_t float_startx,
			uint16_t float_starty,
			float num);
void ui_data_packed_int (
			int_data_struct_t *int_data,
			uint8_t int_name[3],
			uint16_t operate_tpye,
			uint16_t layer,
			uint16_t color,
			uint16_t int_size,
			uint16_t int_width,
			uint16_t int_startx,
			uint16_t int_starty,
			int32_t num);


void ui_delete(type_graphic_delete_e type, uint8_t layer);
void ui_sendcode(
			uint8_t code_name[3],
			uint16_t operate_tpye,
			uint16_t layer,
			uint16_t color,
			uint16_t code_size,
			uint16_t code_width,
			uint16_t code_startx,
			uint16_t code_starty,
			uint16_t code_length,
			uint8_t* CODE);
void ui_sendgraph(Judgesend_length_e data_length, uint8_t* data_locate, client_data_cmd_e date_type);
void ui_sendmessage(uint16_t Robot_Target_ID, client_data_cmd_e CLIENT_INTERACTIVE_CMD_ID, uint8_t* comm_date);
void data_pack_map_position(float target_position_x, float target_position_y, float target_position_z, uint8_t commd_keyboard, uint16_t target_robot_ID);

void judge_send_task(void const *argu);
void ui_init(void);
void ui_test (void);

void ui_mode_display(func_mode_e function);
void ui_block_display(block_store_e * ui_block_state);
void ui_help_display(assist_action_e assist_state);
//void ui_heal_display(heal_action_e heal_state);
void ui_tips_display(void);
#endif

