#include "stm32f10x.h"                  // Device header



int16_t AD_Value[4];

void AD_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //开DMA
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);   //ADC都是APB2的设备，所以用APB2
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   
	 RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //这个PCLK2就是APB2的意思。这里是在开ADCCLK的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  //AIN模式下，GPIO口是无效的。这是为了防止GPIO口的输入输出对模拟电压
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;      //造成干扰
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//规则组的输入通道.理论上可以增加16个通道，就像餐厅的菜单一样
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);

	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //数据对齐。左右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //触发控制的触发源。这里使用软件触发
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  //工作模式。独立/双ADC
	ADC_InitStructure.ADC_NbrOfChannel = 4;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;  //点了四个菜，用ENABLE的多扫描模式了
	ADC_Init(ADC1, &ADC_InitStructure);
		
		
	/*直接加入DMA初始化*/	
	/*ADC把菜做好，DMA把菜端出去*/
	DMA_InitTypeDef DMA_InitStructure;    
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;  //外设站点的起始地址、数据宽度、是否自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;   //存储器站点的起始地址、数据宽度、是否自增
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //传输方向
	DMA_InitStructure.DMA_BufferSize = 4;   //缓存区大小（传输计数器）
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //传输模式（是否使用自动重装）
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //是否选择存储器到存储器
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  //优先级
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_GetCalibrationStatus(ADC1);  //复位校准
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);  //返回复位校准的状态
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}

//uint16_t AD_GetValue(void)
//{
//	DMA_Cmd(DMA1_Channel1, DISABLE);
//	DMA_SetCurrDataCounter(DMA1_Channel1, 4);
//	DMA_Cmd(DMA1_Channel1, ENABLE);
//		
//	while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
//	DMA_ClearFlag(DMA1_FLAG_TC1);
//}


























