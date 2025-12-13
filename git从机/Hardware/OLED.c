#include "stm32f10x.h"
#include "OLED_Font.h"
#include "OLED_I2C.h"  // 包含新的I2C头文件

/* 私有延时函数（用于OLED初始化延时，不依赖外部Delay库） */
static void OLED_Delay(uint32_t count)
{
    volatile uint32_t i;
    for(i = 0; i < count; i++);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
    uint8_t retry = 3;
    
    while(retry--)
    {
        OLED_I2C_Start();
        
        // 发送OLED地址(0x78)并检查应答
        if(OLED_I2C_SendByte(0x78) != 0)
        {
            // 地址无应答，可能是OLED未连接或地址错误
            OLED_I2C_Stop();
            OLED_Delay(100);
            continue;
        }
        
        // 发送控制字节(0x00表示写命令)并检查应答
        if(OLED_I2C_SendByte(0x00) != 0)
        {
            OLED_I2C_Stop();
            OLED_Delay(100);
            continue;
        }
        
        // 发送命令字节并检查应答
        if(OLED_I2C_SendByte(Command) != 0)
        {
            OLED_I2C_Stop();
            OLED_Delay(100);
            continue;
        }
        
        // 所有字节都收到应答，发送成功
        OLED_I2C_Stop();
        return;
    }
    
    // 如果执行到这里，说明所有重试都失败了
    // 可以在这里添加错误处理代码，例如闪烁LED指示错误
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
    uint8_t retry = 3;
    
    while(retry--)
    {
        OLED_I2C_Start();
        
        // 发送OLED地址(0x78)
        if(OLED_I2C_SendByte(0x78) != 0)
        {
            OLED_I2C_Stop();
            OLED_Delay(100);
            continue;
        }
        
        // 发送控制字节(0x40表示写数据)
        if(OLED_I2C_SendByte(0x40) != 0)
        {
            OLED_I2C_Stop();
            OLED_Delay(100);
            continue;
        }
        
        // 发送数据字节
        if(OLED_I2C_SendByte(Data) != 0)
        {
            OLED_I2C_Stop();
            OLED_Delay(100);
            continue;
        }
        
        OLED_I2C_Stop();
        return;
    }
    
    // 发送失败，可添加错误处理
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                    // 设置Y位置
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));    // 设置X位置高4位
    OLED_WriteCommand(0x00 | (X & 0x0F));           // 设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for(i = 0; i < 128; i++)
        {
            OLED_WriteData(0x00);
        }
    }
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      
    uint8_t i;
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);       // 设置光标位置在上半部分
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i]);          // 显示上半部分内容
    }
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);   // 设置光标位置在下半部分
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);      // 显示下半部分内容
    }
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)                            
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0)
    {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++)                            
    {
        OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++)                            
    {
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10)
        {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        }
        else
        {
            OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)                            
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
    }
}

/**
  * @brief  OLED显示角度值
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Angle 要显示的角度值
  * @retval 无
  */
void OLED_ShowAngle(uint8_t Line, uint8_t Column, float Angle)
{
    int32_t value = (int32_t)(Angle * 100 + 0.5); // 放大100倍并四舍五入
    
    // 处理符号
    if(value < 0)
    {
        OLED_ShowChar(Line, Column, '-');
        value = -value;
    }
    else
    {
        OLED_ShowChar(Line, Column, '+');
    }
    
    // 显示整数部分（最多3位）
    OLED_ShowNum(Line, Column + 1, value / 100, 3);
    
    // 显示小数点
    OLED_ShowChar(Line, Column + 4, '.');
    
    // 显示两位小数
    uint8_t decimal = value % 100;
    if(decimal < 10)
    {
        OLED_ShowChar(Line, Column + 5, '0');
        OLED_ShowNum(Line, Column + 6, decimal, 1);
    }
    else
    {
        OLED_ShowNum(Line, Column + 5, decimal, 2);
    }
    
    // 显示度符号（可选）
    OLED_ShowChar(Line, Column + 7, 0xDF);
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
    // 长延时确保OLED电源稳定（约30ms）
    OLED_Delay(30000);
    
    // 使用新的I2C初始化
    OLED_I2C_Init();
    
    // OLED初始化序列 - 每个命令后添加适当延时
    
    OLED_WriteCommand(0xAE);  // 关闭显示
    OLED_Delay(1000);
    
    OLED_WriteCommand(0xD5);  // 设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(0x80);
    OLED_Delay(500);
    
    OLED_WriteCommand(0xA8);  // 设置多路复用率
    OLED_WriteCommand(0x3F);
    OLED_Delay(500);
    
    OLED_WriteCommand(0xD3);  // 设置显示偏移
    OLED_WriteCommand(0x00);
    OLED_Delay(500);
    
    OLED_WriteCommand(0x40);  // 设置显示开始行
    OLED_Delay(500);
    
    OLED_WriteCommand(0xA1);  // 设置左右方向，0xA1正常 0xA0左右反置
    OLED_Delay(500);
    
    OLED_WriteCommand(0xC8);  // 设置上下方向，0xC8正常 0xC0上下反置
    OLED_Delay(500);
    
    OLED_WriteCommand(0xDA);  // 设置COM引脚硬件配置
    OLED_WriteCommand(0x12);
    OLED_Delay(500);
    
    OLED_WriteCommand(0x81);  // 设置对比度控制
    OLED_WriteCommand(0xCF);
    OLED_Delay(500);
    
    OLED_WriteCommand(0xD9);  // 设置预充电周期
    OLED_WriteCommand(0xF1);
    OLED_Delay(500);
    
    OLED_WriteCommand(0xDB);  // 设置VCOMH取消选择级别
    OLED_WriteCommand(0x30);
    OLED_Delay(500);
    
    OLED_WriteCommand(0xA4);  // 设置整个显示打开/关闭
    OLED_Delay(500);
    
    OLED_WriteCommand(0xA6);  // 设置正常/倒转显示
    OLED_Delay(500);
    
    OLED_WriteCommand(0x8D);  // 设置充电泵
    OLED_WriteCommand(0x14);
    OLED_Delay(500);
    
    OLED_WriteCommand(0xAF);  // 开启显示
    OLED_Delay(2000);         // 长延时确保显示稳定
    
    // 清屏
    OLED_Clear();
    OLED_Delay(1000);
}
