
#include "pubdata.h"

/* 管脚拉高 */
void DS18B20_DQ_High(void)
{
	GPIO_SetBits(GPIO_DS18B20, DS18B20_Pin);
}

/* 管脚拉低 */
void DS18B20_DQ_Low(void)
{
	GPIO_ResetBits(GPIO_DS18B20, DS18B20_Pin);
}

/* 配置为上拉输入模式 */
void DS18B20_IO_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin  = DS18B20_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  

	GPIO_Init(GPIO_DS18B20, &GPIO_InitStruct);
}

/* 配置为推挽输出模式 */
void DS18B20_IO_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin   = DS18B20_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;  

	GPIO_Init(GPIO_DS18B20, &GPIO_InitStruct);
}

/* 读取字节 */
u8 DS18B20_Read_Byte(void)
{
	u8 i = 0, TempData = 0;

	for ( i = 0; i < 8; i++ )
	{
		TempData >>= 1;	

		DS18B20_IO_OUT();  /* 设置为输出模式 */
		DS18B20_DQ_Low();  /* 管脚拉低 */
		delay_us(4);      
		DS18B20_DQ_High(); /* 管脚拉高, 释放总线 */
		delay_us(10); 
		DS18B20_IO_IN();   /* 设置为输入模式 */

		/* 读取到的是高电平 */
		if ( GPIO_ReadInputDataBit(GPIO_DS18B20, DS18B20_Pin) == 1 )
		{
			TempData |= 0x80;	
		}

		delay_us(45);
	}

	return TempData;
}

/* 写字节函数 */
void DS18B20_Write_Byte(u8 data)
{
	u8 i = 0;

	/* 设置为输出模式 */
	DS18B20_IO_OUT();  
	
	for ( i = 0; i < 8; i++ )
	{
		DS18B20_DQ_Low();   /* 管脚拉低 */
		delay_us(15);  

		/* 写一个 bit */
		GPIO_WriteBit(GPIO_DS18B20, DS18B20_Pin, data & 0x01);
		
		delay_us(60);
		
		DS18B20_DQ_High();
		
		data >>= 1;
	}	
}

/* 芯片复位 */
u8 DS18B20_Reset(void)
{
	u8 count = 0;
	
	DS18B20_IO_OUT();  /* 管脚配置为输出模式 */
	DS18B20_DQ_Low();  /* 拉低管脚 */
	delay_us(480);     /* 延时 480us */
	DS18B20_DQ_High(); /* 管脚拉高 */
	delay_us(80);      /* 延时80us */

	DS18B20_IO_IN();

	/* 等待设备响应 */
	while ( GPIO_ReadInputDataBit(GPIO_DS18B20, DS18B20_Pin))
	{
		count++;

		delay_us(1);

		/* 响应超时 */
		if ( count > 60 )
		{
			return 1;
		}
	}

	delay_us(400); 

	return 0;
}

u8 DS18B20_GetID(u8 *ID)
{
	u8 i = 0, ret = 0;
	
	ret = DS18B20_Reset();     /* 复位 */

	/* 没有设备 */
	if ( ret == 1 )
	{
		return 1;
	}
	
	DS18B20_Write_Byte(0x33);  /* 读取ID */
	
	delay_ms(800);
		
	for ( i = 0; i < 8; i++ )
	{
		ID[i] = DS18B20_Read_Byte();
	}

	return 0;
}

/* 计算温度 */
u8 DS18B20_GetTemp(u8 *ID, double *temp)
{	
	u8  TL = 0, TH = 0;
	u8  ret = 0, i;
	u16 Temp  = 0;
	
	ret = DS18B20_Reset(); 

	if ( ret == 1 )
	{
		return 1;
	}
	
	DS18B20_Write_Byte(0x55);  /* 符合指定ID号的传感器 */
	
	for ( i = 0; i < 8; i++ )
	{
		DS18B20_Write_Byte(ID[i]);
	}
	
	DS18B20_Write_Byte(0x44);  /* 温度转换命令 */

	delay_ms(800);

	ret = DS18B20_Reset(); 

	if ( ret == 1 )
	{
		return 1;
	}
	
	DS18B20_Write_Byte(0x55);  /* 符合指定ID号的传感器 */
	
	for ( i = 0; i < 8; i++ )
	{
		DS18B20_Write_Byte(ID[i]);
	}
	
	DS18B20_Write_Byte(0xBE);  /* 读温度命令 */

	TL = DS18B20_Read_Byte();  /* 读取低字节 */
	TH = DS18B20_Read_Byte();  /* 读取高字节 */

	Temp = (TH << 8) + TL;

	/* 前5位为1,表示温度为负 */
	if ( (Temp & 0xF800) == 0xF800 ) 
	{
		Temp  = ~Temp + 1;         /* 负数补码取反加 1 */
		*temp = Temp * (-0.0625);
	}
	else
	{
		*temp = Temp * 0.0625;
	}

	return 0;
}

