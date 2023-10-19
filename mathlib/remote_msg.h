#ifndef __REMOTE_MSG_H__
#define __REMOTE_MSG_H__

#include "stm32f4xx_hal.h"

/* remote control information structure */
typedef __packed struct
{
  /* rocker channel information */
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;
	int16_t ch5;
  /* left and right lever information */
  uint8_t sw1;
  uint8_t sw2;
  /* mouse movement and button information */
  __packed struct
  {
    int16_t x;
    int16_t y;
    int16_t z;
  
    uint8_t l;
    uint8_t r;
  } mouse;
  /* keyboard key information */
  __packed union
  {
    uint16_t key_code;
    __packed struct 
    {
      uint16_t W:1;
      uint16_t S:1;
      uint16_t A:1;
      uint16_t D:1;
      uint16_t SHIFT:1;
      uint16_t CTRL:1;
      uint16_t Q:1;
      uint16_t E:1;
      uint16_t R:1;
      uint16_t F:1;
      uint16_t G:1;
      uint16_t Z:1;
      uint16_t X:1;
      uint16_t C:1;
      uint16_t V:1;
      uint16_t B:1;
    } bit;
  } kb;
} rc_info_t;

typedef enum 
{
	 KB_W			=0,
	 KB_S			=1,
	 KB_A			=2,
	 KB_D			=3,
	 KB_SHIFT	=4,
	 KB_CTRL	=5,
	 KB_Q			=6,
	 KB_E			=7,
	 KB_R			=8,
	 KB_F			=9,
	 KB_G			=10,
	 KB_Z			=11,
	 KB_X			=12,
	 KB_C			=13,
	 KB_V			=14,
	 KB_B			=15,
	 mouse_L	=16,
	 mouse_R	=17,	
}button_bit;

enum
{
  RC_UP = 1,
  RC_MI = 3,
  RC_DN = 2,
};

#define DBUS_MAX_LEN 50
#define DBUS_BUFLEN  18

extern rc_info_t   rc;

void rc_callback_handler(rc_info_t *rc, uint8_t *buff);
int oneclick_verify(button_bit bit);

#define IF_W      (oneclick_verify(KB_W))
#define IF_S      (oneclick_verify(KB_S))
#define IF_A      (oneclick_verify(KB_A))
#define IF_D      (oneclick_verify(KB_D))
#define IF_SHIFT  (oneclick_verify(KB_SHIFT))
#define IF_CTRL   (oneclick_verify(KB_CTRL))
#define IF_Q      (oneclick_verify(KB_Q))
#define IF_E      (oneclick_verify(KB_E))
#define IF_R      (oneclick_verify(KB_R))
#define IF_F      (oneclick_verify(KB_F))
#define IF_G      (oneclick_verify(KB_G))
#define IF_Z      (oneclick_verify(KB_Z))
#define IF_X      (oneclick_verify(KB_X))
#define IF_C      (oneclick_verify(KB_C))
#define IF_V      (oneclick_verify(KB_V))
#define IF_B      (oneclick_verify(KB_B))
#define IF_ML     (oneclick_verify(mouse_L))
#define IF_MR     (oneclick_verify(mouse_R))

#endif
