
#ifndef __DS18B20_H
#define __DS18B20_H

#include "pubdata.h"

#define DS18B20_Pin    GPIO_Pin_0
#define GPIO_DS18B20   GPIOA

void DS18B20_DQ_High(void);
void DS18B20_DQ_Low(void);
void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);
void DS18B20_Write_Byte(u8 data);
u8 DS18B20_Read_Byte(void);
u8 DS18B20_Reset(void);
u8 DS18B20_GetID(u8 *ID);
u8 DS18B20_GetTemp(u8 *ID, double *temp);

#endif

