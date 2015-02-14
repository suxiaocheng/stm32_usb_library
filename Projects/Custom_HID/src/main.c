/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Custom HID demo main file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "debug.h"
#include "sys_timer.h"
#include "stdio.h"
#include "key.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
__IO uint8_t PrevXferComplete = 1;
__IO uint32_t TimingDelay = 0;
extern uint32_t __Vectors;
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
	uint32_t key_value;
	uint8_t usb_buffer[2];
	const uint8_t key_report_id_list[] = {5, 6};
	
	/* if debug in ram, reset the interrupt vectors to ram area */
#ifdef VECT_TAB_SRAM
	SCB->VTOR = (uint32_t) & __Vectors | SCB_VTOR_TBLBASE;
#else
	SCB->VTOR = (uint32_t) & __Vectors;
#endif

	/* enable the timer and the uart debug function */
	init_sys_timer();
	init_debug_fun();

	stm_printf("startup\n");
	Set_System();
	//key_init();

	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
	
	while (1) {
		#if 0
		if(sys_timer_20ms_flag == TRUE){
			sys_timer_20ms_flag = FALSE;
			key_value = key_scan();
			/* short press, send the key press message to the usb */
			if(key_value & (KEY_STAT_VALID|KEY_STAT_UP)){
				stm_printf("key:%x\n", key_value);
				//__disable_irq();
				if ((PrevXferComplete) && (bDeviceState == CONFIGURED)) {
					PrevXferComplete = 0;
					if((key_value & KEY_MSK) < 0x2){
						usb_buffer[0] = key_report_id_list[key_value & KEY_MSK];
						if(key_value & KEY_STAT_VALID){
							usb_buffer[1] = 0x01;
						}else if(key_value & (KEY_STAT_UP|KEY_STAT_LONG_UP)){
							usb_buffer[1] = 0x00;
						}
						/* Write the descriptor through the endpoint */
						USB_SIL_Write(EP1_IN, (uint8_t *) usb_buffer, 2);
						SetEPTxValid(ENDP1);
					}
				}
				//__enable_irq();
			}
		}
		#endif
	}
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
	TimingDelay = nCount;
	for (; nCount != 0; nCount--) ;
}

#ifdef  USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t * file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1) {
	}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
