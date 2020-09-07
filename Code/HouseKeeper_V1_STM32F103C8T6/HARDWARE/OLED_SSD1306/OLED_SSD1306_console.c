#include "OLED_SSD1306_console.h"
#include "OLED_SSD1306.h"



//定义光标的限制
#define COLUMN_8_NUM  	128/8
#define ROW_16_NUM 		64/16
//光标位置
u8 cursorX = 0;
u8 cursorY = 0;
/* 类似控制台显示模式，自动换行，清屏功能 */
void OLED_show_console_16(unsigned char* str)
{
	OLED_RamClear();
	cursorX = 0;
	cursorY = 0;
	u8 k;
	while(*str)
	{       
		if((*str) < 128) // ASC段
		{
			k = *str;
			if (k == 13) //回车\r\n
			{
				cursorX += 2;
				if(cursorX >= COLUMN_8_NUM)
				{
					cursorX = 0; 
					cursorY ++;
					
					if(cursorY == ROW_16_NUM){
						cursorY = 0;
						//OLED_RamClear();
						OLED_Fill(0, 0, 127, 16, 0);
					}
					else
						OLED_Fill(0, cursorY * 16, 127, (cursorY+1) * 16, 0);
						
				}
				
				str += 2;
			}
			else
			{
				OLED_ShowChar(cursorX * 8, cursorY * 16, *str, 16, 1);
				cursorX++;
				str ++;
			}
			
		}
		else         // 汉字段
		{
			str ++;
		}
		if(cursorX == COLUMN_8_NUM)
		{
			cursorX = 0; 
			cursorY ++;
			
			if(cursorY == ROW_16_NUM){
				cursorY = 0;
				//OLED_RamClear();
				OLED_Fill(0, 0, 127, 16, 0);
			}
			else
				OLED_Fill(0, cursorY * 16, 127, (cursorY+1) * 16, 0);
				
		}
		
	}       
}


void OLED_show_console_16_d(unsigned char* str)
{
	u8 k;
	while(*str)
	{       
		if((*str) < 128) // ASC段
		{
			k = *str;
			if (k == 13) //回车\r\n
			{
				cursorX = 0;
				cursorY ++;
				
				if(cursorY == ROW_16_NUM){
					cursorY = 0;
					//OLED_RamClear();
					OLED_Fill(0, 0, 127, 16, 0);
				}
				else
					OLED_Fill(0, cursorY * 16, 127, (cursorY+1) * 16, 0);
				str += 2;
			}
			else
			{
				OLED_ShowChar(cursorX * 8, cursorY * 16, *str, 16, 1);
				cursorX++;
				str ++;
			}
			
		}
		else         // 汉字段
		{
			str ++;
		}
		if(cursorX == COLUMN_8_NUM)
		{
			cursorX = 0; 
			cursorY ++;
			
			if(cursorY == ROW_16_NUM){
				cursorY = 0;
				//OLED_RamClear();
				OLED_Fill(0, 0, 127, 16, 0);
			}
			else
				OLED_Fill(0, cursorY * 16, 127, (cursorY+1) * 16, 0);
				
		}
		
	}       
}

void OLED_printf(unsigned char* str)
{
	OLED_show_console_16(str);
	OLED_Refresh_OneTime();
}



