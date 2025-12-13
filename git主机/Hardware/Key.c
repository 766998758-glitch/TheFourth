#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint8_t Key_Num;
uint8_t Key_LongPress = 0;           
uint8_t Key_LongPress_Active = 0;    

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

uint8_t Key_GetNum(void)    
{
	uint8_t temp;  //我不能直接返回Key_Nun,要不然这家伙在我松手后就一直是非零，传到mode函数那里，灯就会疯狂切换模式闪烁
	temp = Key_Num;
	Key_Num = 0;
	return temp;
}
uint8_t Key_GetLongPress(void) //上面哪那个获取普通按键，这个获取长按按键
{
    uint8_t temp;
    temp = Key_LongPress;
    Key_LongPress = 0;
    return temp;
}


uint8_t Key_GetState(void) //获取当前按键状态
{
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)  
	{
		return 1;
	}
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)  
	{
		return 2;
	}
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)  
	{
		return 3;
	}
		return 0;   //如果没有任何一个按键按下
}

void Key_Tick(void){         //你的第一个”状态机“，用于配合TIM中断来消除抖动
	static uint16_t count;
	static uint8_t CurrState, PrevState;   //static初始值是0
	static uint16_t long_press_count = 0;     //长按计时
  static uint16_t repeat_count = 0;       //重复触发计时
	static uint8_t long_press_key = 0;         //当前长按的按键
	
	count++;
	
	if(count>=20)       //这个20指20ms，我们用的是20ms中断一次
	{
		count=0;
		PrevState = CurrState;
		CurrState = Key_GetState();
		
		if(CurrState == 0 && PrevState != 0)  //捕捉到一次按键按下再松手
		{
			if(long_press_count < 1000)
			{
				Key_Num = PrevState;     
			}
					long_press_count = 0;
          repeat_count = 0;
          Key_LongPress_Active = 0;
          long_press_key = 0;
		}
		
		if(CurrState == 1 || CurrState == 2)  
        {
            long_press_count += 20;  
            
            if(long_press_count >= 1000 && long_press_key == 0)
            {
                Key_LongPress = CurrState;  
                Key_LongPress_Active = 1;   
                long_press_key = CurrState; 
                repeat_count = 0;           
            }
        }
        else
        {
            long_press_count = 0;
            Key_LongPress_Active = 0;
            long_press_key = 0;
        }
        
        if(Key_LongPress_Active && (CurrState == 1 || CurrState == 2))
        {
            repeat_count += 20;
            if(repeat_count >= 100)  
            {
                Key_LongPress = long_press_key;  
                repeat_count = 0;                
            }
        }
    }
}
		
		
	
	























