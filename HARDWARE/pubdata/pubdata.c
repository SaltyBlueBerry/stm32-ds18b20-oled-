
#include "pubdata.h"

/*
 * SysTick 属于嵌套中断向量NVIC控制部分一共有4个寄存器
 * STK_CSR  : 0xE000E010  -- 控制寄存器
 * STK_LOAD : 0xE000E014  -- 重载寄存器
 * STK_VAL  : 0xE000E018  -- 当前值寄存器
 * STK_CALR : 0xE000E01C  -- 校准值寄存器
 */


int fputc(int ch, FILE *f)
{
	/* 发送数据 */
	USART_SendData(USART2, (u8)ch);

	/* 等待发送完毕 */
	while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET );

	return ch;
}


