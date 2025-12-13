
#include "stm32f10x.h"
#include "OLED_I2C.h"

static void OLED_Delay(uint32_t count)
{
    volatile uint32_t i;
    for(i = 0; i < count; i++);
}

#define OLED_SCL_HIGH()    (GPIOB->BSRR = GPIO_Pin_6)
#define OLED_SCL_LOW()     (GPIOB->BRR = GPIO_Pin_6)
#define OLED_SDA_HIGH()    (GPIOB->BSRR = GPIO_Pin_7)
#define OLED_SDA_LOW()     (GPIOB->BRR = GPIO_Pin_7)
#define OLED_SDA_READ()    ((GPIOB->IDR & GPIO_Pin_7) ? 1 : 0)

void OLED_I2C_Init(void)
{

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    

    GPIOB->CRL &= ~(0xFF << 24); 
    GPIOB->CRL |= (0x03 << 24) | (0x03 << 28);  
    
    OLED_SDA_HIGH();
    OLED_SCL_HIGH();
    OLED_Delay(1000);
    
    GPIOB->CRL &= ~(0xFF << 24);  
    GPIOB->CRL |= (0x06 << 24) | (0x06 << 28);
    
    OLED_SDA_HIGH();
    OLED_SCL_HIGH();
    OLED_Delay(1000);
}


void OLED_I2C_Start(void)
{
    OLED_SDA_HIGH();
    OLED_SCL_HIGH();
    OLED_Delay(10);
    
    OLED_SDA_LOW();
    OLED_Delay(10);
    
    OLED_SCL_LOW();
    OLED_Delay(10);
}


void OLED_I2C_Stop(void)
{
    OLED_SDA_LOW();
    OLED_Delay(10);
    
    OLED_SCL_HIGH();
    OLED_Delay(10);
    
    OLED_SDA_HIGH();
    OLED_Delay(10);
}


uint8_t OLED_I2C_SendByte(uint8_t data)
{
    uint8_t i, ack;
    
    for(i = 0; i < 8; i++)
    {
        if(data & 0x80)
            OLED_SDA_HIGH();
        else
            OLED_SDA_LOW();
        
        OLED_Delay(5);        
        OLED_SCL_HIGH();      
        OLED_Delay(10);         
        OLED_SCL_LOW();  
        OLED_Delay(5);          
        
        data <<= 1;
    }
    
    OLED_SDA_HIGH();           
    OLED_Delay(5);
    OLED_SCL_HIGH();          
    OLED_Delay(10);
    
    ack = OLED_SDA_READ();     
    OLED_SCL_LOW();          
    OLED_Delay(5);
    
    return ack; 
}


uint8_t OLED_I2C_ReceiveByte(uint8_t ack)
{
    uint8_t i, data = 0;
    
    OLED_SDA_HIGH();          
    
    for(i = 0; i < 8; i++)
    {
        OLED_Delay(5);
        OLED_SCL_HIGH();       
        OLED_Delay(10);
        
        data <<= 1;
        if(OLED_SDA_READ())
            data |= 0x01;
        
        OLED_SCL_LOW();        
        OLED_Delay(5);
    }
    
    if(ack)
        OLED_SDA_HIGH();       
    else
        OLED_SDA_LOW();        
    
    OLED_Delay(5);
    OLED_SCL_HIGH();          
    OLED_Delay(10);
    OLED_SCL_LOW();             
    OLED_Delay(5);
    
    OLED_SDA_HIGH();         
    
    return data;
}
