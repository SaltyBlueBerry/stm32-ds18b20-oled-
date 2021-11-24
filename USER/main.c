#include "led.h"
#include "delay.h"
#include "sys.h"
//#include "usart.h"
#include "oled.h"
#include "pubdata.h"
#include "ds18b20.h"   
//ALIENTEK Mini STM32开发板范例代码23
//DS18B20数字温度传感器实验  
//ds18b20.c .h来源于csdn下载（https://download.csdn.net/download/zhou0842/9699352）如有知道原作者欢迎告知
//
////////////////////////////////////////////////
//
//			修改者：berry 2021.11.23
//			使用说明： oled 接在 PB13（SCL） PB15（SDA）
//			ds18b20使用单总线模式，请务必提前读取自己的传感器的地址（debug模式可以看到）
//			地址：每次单独接一个传感器，在debug里查看ID的值就是它的地址（断点设置在while1就可以）
//			理论可以接很多很多传感器，所有传感器接在PA0引脚上
//
///////////////////////////////////////////////
//
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
 int main(void)
 { 
	u8 t=0;		
	u8 flag;	 
	short temperature; 
	double temp;
	u8 ID[8], i;
	u8 ID1[8] = {0x28,0xFF,0XE2,0X93,0X67,0X18,0X01,0X63};//传感器的64位地址
	u8 ID2[8] = {0x28,0xee,0x1b,0x1a,0x33,0x16,0x01,0x29};
	u8 ID3[8] = {0x28,0xff,0x7d,0x93,0x67,0x18,0x01,0xbb};
	u8 ID4[8] = {0x28,0xee,0x8c,0x49,0x33,0x16,0x01,0x9f};
	u8 temps[4];
	//u8 IDS[4] = {&ID1,&ID2,&ID3,&ID4};
	u8* IDS[4];//存放4路传感器的地址
	u32 id;

	u8 ret = 0;	 
	delay_init();	    	 //延时函数初始化	  
	//uart_init(9600);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
	
	 OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	 	OLED_Refresh();
	 OLED_Clear();

	
	
	ret = DS18B20_GetID(ID);//每次单独接一个传感器，在debug里查看ID的值就是它的地址
	
	if ( ret == 1 )
	{
		OLED_ShowString(0,0,"not find ds18b20",16);
	}
	else
	{

		
	OLED_ShowString(0,0,"ID1:",16);
	
	for ( i = 0; i < 8; i++ )
		{
			
			OLED_ShowNum(30+i*9,0,ID1[i],1,16);
		}
	
	}
	

	
	
	OLED_Refresh();//更新显存到OLED	
	
		
	
	IDS[0] = ID1;//我猜是会把ID1[]数组的起始地址放进去
	IDS[1] = ID2;
	IDS[2] = ID3;
	IDS[3] = ID4;
	
	
	
	
	
	
	
while(1)
	{	    	   


	
	for ( i = 0; i < 4; i++ )//遍历4路传感器
		{
			

		ret = DS18B20_GetTemp(IDS[i], &temp);//这个函数接收的好像是一个数组的起始地址

		if ( ret == 1 )
		{	
			//printf("没有发现设备!\r\n");
			OLED_ShowString(0,16,"no sd18b20!",16);
			OLED_Refresh();//更新显存到OLED	
			delay_ms(500);

		}
		else
		{
			//printf("当前环境温度 : %0.4f ℃\r\n", temp);
				temp=(int)temp;//类型转换
			OLED_ShowString(0,16,"temp",16);
			OLED_ShowNum(35,16,i+1,1,16);
			OLED_ShowString(45,16,":",16);
			OLED_ShowNum(55,16,temp,2,16);
			OLED_Refresh();//更新显存到OLED	
			temps[i] = temp;
		}

		OLED_Refresh();//更新显存到OLED	

	//	LED0=!LED0;
		delay_ms(20);
	}

	
	
	
	
	//报警功能/////////////////////////////////////////////////////////
	//
	//当任意一路温度超过23度时报警，oled显示报警序号（位置）
	//
	//独立的两个for i不会产生干扰，若要改动请注意变量i问题
	OLED_ShowString(0,35,"alarm:",16);
	OLED_ShowString(55,35,"     ",16);
	flag = 0;
	for ( i = 0; i < 4; i++ )
		{
			if(temps[i] > 23)
			{
				flag = 1;
				
				OLED_ShowNum(55+i*9,35,i+1,1,16);
				
			}	
			
		}
	OLED_Refresh();//更新显存到OLED	
	LED0 = !flag; //取反flag
		
		//报警功能////////////////////////////////////////////////////////////////////
		
		
		
	}
}
