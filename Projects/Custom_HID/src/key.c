#include "key.h"

__IO uint16_t ADCConvertedValue[8];
__IO key_info_t key_info = {0, NULL, FALSE};

const uint16_t key_adc_value[] = {0, 0x430, 0x790};
const uint16_t key_msg_value[4] = {KEY_PREV, KEY_MODE, KEY_NEXT, KEY_NULL};

uint32_t key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    /* PA0->ADC_KEY  PA3->ADC_BATTARY */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* DMA1 channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 3;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    
    /* Enable DMA1 channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
    
    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 3;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_TempSensorVrefintCmd(ENABLE);
    
    /* ADC1 regular channel0 configuration */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
    /* ADC1 regular channel3 configuration */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_13Cycles5);
    /* ADC1 regular temperature configuration */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 3, ADC_SampleTime_41Cycles5);
    
    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
    
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    
    /* Enable ADC1 reset calibration register */   
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));
    
    /* Start ADC1 calibration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));
    
    /* Start ADC1 Software Conversion */ 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
    /* init the key info register */
    key_info.count = 0;
    key_info.last_key = NULL;
    key_info.status = TRUE;
    
    return 0;
}

uint32_t key_get_adc_value(uint16_t *adc_val)
{
	uint32_t ret = FALSE;
	*adc_val = ADCConvertedValue[0];
	return ret;
}

uint32_t battary_get_adc_value(uint16_t *adc_val)
{
	uint32_t ret = FALSE;
	*adc_val = ADCConvertedValue[1];
	return ret;
}

uint32_t temperature_get_adc_value(uint16_t *adc_val)
{
	uint32_t ret = FALSE;
	*adc_val = ((int32_t)1430 - (int32_t)ADCConvertedValue[2]*3300/4096)*100/43 + 250;
	return ret;
}

uint32_t key_scan(void)
{
	uint32_t current_key = KEY_NULL;
	uint16_t adc_value;
	uint32_t i;
	if(key_info.status != TRUE){
		return 0;
	}
	key_get_adc_value(&adc_value);
	
	//stm_printf("adc_value:%x\n", adc_value);
	
	for(i=0; i<(sizeof(key_adc_value)/sizeof(uint16_t)); i++){
		if((adc_value > key_adc_value[i]) && \
			(adc_value < (key_adc_value[i])+0x100)){
			current_key = key_msg_value[i];
			break;
		}
	}

	if(current_key != KEY_NULL){
		if(current_key == key_info.last_key){
			key_info.count++;
			if((key_info.count % 16) == 0){
				current_key = key_info.last_key;
				if(key_info.count == 16){
					current_key |= KEY_STAT_LONG;
				}else{
					current_key |= KEY_STAT_KEEP_LONG;
				}
			}
			if(key_info.count == 3){
				current_key |= KEY_STAT_VALID;
			}
		}else{
			key_info.last_key = current_key;
			key_info.count = 0;
		}
	}else{
		if(key_info.last_key){
			if(key_info.count >= 16){
				current_key = key_info.last_key | KEY_STAT_LONG_UP;
			}else if(key_info.count > 3){
				current_key = key_info.last_key | KEY_STAT_UP;
			}
			key_info.last_key = KEY_NULL;
			key_info.count = 0;
		}
	}
	
	return (current_key&(KEY_STAT_LONG_UP|KEY_STAT_UP|KEY_STAT_LONG|KEY_STAT_KEEP_LONG|KEY_STAT_VALID))?current_key:0;
}
