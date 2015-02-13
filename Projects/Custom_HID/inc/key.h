#ifndef KEY_H
#define KEY_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f10x.h"
#include "sys_timer.h"
#include "debug.h"
#include "misc_lib.h"
	 

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE	0
#endif
	 
#define KEY_STAT_UP					((uint32_t)0x1<<31)
#define KEY_STAT_LONG				((uint32_t)0x1<<30)
#define KEY_STAT_KEEP_LONG			((uint32_t)0x1<<29)
#define KEY_STAT_LONG_UP			((uint32_t)0x1<<28)
#define KEY_STAT_VALID				((uint32_t)0x1<<27)
	 
#define KEY_NULL						0x0
#define KEY_PREV						0x1
#define KEY_NEXT						0x2
#define KEY_MODE						0x3
#define KEY_MSK							0xffff
	 
typedef struct{
	uint32_t last_key;
	uint32_t count;
	uint32_t status;
}key_info_t;
	 

uint32_t key_init(void);
uint32_t key_get_adc_value(uint16_t *adc_val);
uint32_t battary_get_adc_value(uint16_t *adc_val);
uint32_t temperature_get_adc_value(uint16_t *adc_val);
uint32_t key_scan(void);
	 
#ifdef __cplusplus
 }
#endif 

#endif
