/*
 * user_lcd.h
 *
 *  Created on: 2022年2月19日
 *      Author: jiming
 */

#ifndef APP_INCLUDE_USER_LCD_H_
#define APP_INCLUDE_USER_LCD_H_

#include "osapi.h"
#include "ets_sys.h"
#include "driver/spi_interface.h"
#include "driver/spi.h"
#include "gpio.h"
#include "eagle_soc.h"

//define lcd size
#define LCD_W 128
#define LCD_H 160

//POINT COLOR
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000
#define BLUE         	 0x001F
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430
//GUI COLOR
#define DARKBLUE      	 0X01CF
#define LIGHTBLUE      	 0X7D7C
#define GRAYBLUE       	 0X5458
//PANEL COLOR

#define LIGHTGREEN     	 0X841F
#define LGRAY 			 0XC618

#define LGRAYBLUE        0XA651
#define LBBLUE           0X2B12

#define IO_HIGH		1
#define IO_LOW		0
#define LCD_CS_BIT	BIT15
#define LCD_CS_NO	15
#define LCD_RS_NO	4
#define LCD_AO_NO	5
#define LCD_SDA_NO	13
#define LCD_SCK_NO	14
#define LCD_CS(output_val)	GPIO_OUTPUT_SET(LCD_CS_NO, output_val)
#define LCD_RS(output_val)	GPIO_OUTPUT_SET(LCD_RS_NO, output_val)
#define LCD_AO(output_val)	GPIO_OUTPUT_SET(LCD_AO_NO, output_val)
#define LCD_SDA(output_val)	GPIO_OUTPUT_SET(LCD_SDA_NO, output_val)
#define LCD_SCK(output_val)	GPIO_OUTPUT_SET(LCD_SCK_NO, output_val)
#define LCD_CS_STATUS()		GPIO_INPUT_GET(LCD_CS_NO)
#define SPI_LCD_CS_LOW()	LCD_CS(0)
#define SPI_LCD_CS_HIGH()	LCD_CS(1)
#define SPI_LCD_DATA_W()	LCD_AO(0)
#define SPI_LCD_DATA_R()	LCD_AO(1)
void Lcd_Init();
void LCD_Clear(u16 Color);
void LCD_fillScreen(u16 color);
void LCD_WriteHalfWord(u16 dat16);
void LCD_Writ_Bus(u8 da);

void LCD_DrawPoint(u16 x,u16 y);//
void LCD_DrawPoint_big(u16 x,u16 y);//
u16  LCD_ReadPoint(u16 x,u16 y); //
void Draw_Circle(u16 x0,u16 y0,u8 r);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
#if 1
void LCD_ShowString(u16 x,u16 y,const u8 *p);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode);
#else
void LCD_ShowChar(u16 x, u16 y, char ch, u16 back_color, u16 font_color, u8 font_size);//
#endif
void LCD_ShowCharStr(u16 x, u16 y, char* str, u16 back_color, u16 font_color, u8 font_size);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len);//
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len);//
void LCD_ShowString(u16 x,u16 y,const u8 *p);		 //
void showimage();
void xianshi();
#endif /* APP_INCLUDE_USER_LCD_H_ */
