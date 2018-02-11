#ifndef _SSD1289_H_
#define _SSD1289_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_ll_fsmc.h"

#include <stdlib.h>
#include <string.h>

#define 	 LCD_PIXEL_WIDTH  800
#define 	 LCD_PIXEL_HEIGHT 480

extern const uint8_t font8x8[][8];

#define MIRROR_V 0
#define MIRROR_H 0

#define LCD_RST_SET       HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
#define LCD_RST_RESET     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
											
#define LSHIFT 0

#define CMD_ADR (*(uint16_t *)0x60000000)
#define DAT_ADR (*(uint16_t *)0x60020000) 

#define LCD_WIDTH       480
#define LCD_HEIGHT      800
#define MAX_X LCD_HEIGHT - 1
#define LCD_PIXEL_COUNT	LCD_WIDTH * LCD_HEIGHT

uint16_t RGB(uint8_t r, uint8_t g, uint8_t b);
uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b);
uint16_t RGB565_Reversed(uint8_t r, uint8_t g, uint8_t b);
uint16_t H24_RGB565(uint32_t color24);
uint16_t H24_RGB565_Reversed(uint32_t color24);

#define BLACK 	0x000000 /*   0,   0,   0 */
#define WHITE 	0xFFFFFF /* 255, 255, 255 */
#define RED 		0xFF0000 /* 255,   0,   0 */
#define BLUE 		0x0000FF /*   0,   0, 255 */
#define BLUE_D 	0x0000A0 /*   0,   0, 160 */
#define CYAN 		0x00FFFF /*   0, 255, 255 */
#define CYAN_D	0x008080 /*   0, 128, 128 */
#define YELLOW 	0xFFFF00 /* 255, 255,   0 */
#define MAGENTA 0xFF00FF /* 255,   0, 255 */
#define GREEN 	0x00FF00 /*   0, 255,   0 */
#define GREEN_D 0x008000 /*   0, 128,   0 */
#define PURPLE 	0x800080 /* 128,   0, 128 */
#define TEAL 		0x008080
#define NAVY 		0x000080 /*   0,   0, 128 */
#define SILVER 	0xC0C0C0 /* 192, 192, 192 */
#define GRAY 		0x808080 /* 128, 128, 128 */
#define ORANGE 	0xFFA500 /* 255, 165,   0 */
#define BROWN 	0xA52A2A
#define MAROON 	0x800000 /* 128,   0,   0 */
#define OLIVE 	0x808000 /* 128, 128,   0 */
#define LIME 		0xBFFF00 /* 191, 255,   0 */

typedef struct {
	__IO uint16_t REG;
	__IO uint16_t RAM;

} LCD_CONTROLLER_TypeDef;

#define LCD_BASE ((uint32_t)0x6001FFFE)
#define LCD 		 ((LCD_CONTROLLER_TypeDef *) LCD_BASE)

extern char array [100]; 

void SSD1963_Command(uint16_t com);
void SSD1963_Data(uint16_t data);
void SSD1963_Init(void);
void SSD1963_Bright(uint8_t bright);
void SSD1963_Test(void);

void LCD_CursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Pixel(uint16_t ysta, uint16_t xsta, uint32_t color24);
void LCD_String_Font(uint16_t x0, uint16_t y0, uint32_t ground24, uint32_t color, char *s);
void LCD_Set_XY(uint16_t x, uint16_t y);
void LCD_Set_Work_Area(uint16_t x, uint16_t y, uint16_t length, uint16_t width);
void LCD_Char(uint16_t x, uint16_t y, uint32_t color24, uint32_t ground, const unsigned char *table, uint8_t ascii, uint8_t size);
void LCD_String(uint16_t x, uint16_t y, uint32_t color24, uint32_t ground, const unsigned char *table, char *string, uint8_t size);
void LCD_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color24, uint8_t size);
void LCD_Rectangle(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint8_t size, uint32_t color24);
void LCD_Rectangle_Fill(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint32_t color24);
void LCD_Round_Rect(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint8_t size, uint32_t color24);
void LCD_Round_Rect_Fill(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint32_t color24);
void LCD_Triangle( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t size, uint32_t color24);
void LCD_Circle(uint16_t x, uint16_t y, uint8_t radius, uint8_t fill, uint8_t size, uint32_t color24);

#define LCD_RESET			 		  0x0001
#define LCD_SLEEP_OUT		  	0x0011
#define LCD_GAMMA			    	0x0026
#define LCD_DISPLAY_OFF			0x0028
#define LCD_DISPLAY_ON			0x0029
#define LCD_COLUMN_ADDR			0x002A
#define LCD_PAGE_ADDR			  0x002B
#define LCD_GRAM				    0x002C
#define LCD_MAC			        0x0036
#define LCD_PIXEL_FORMAT    0x003A
#define LCD_WDB			    	  0x0051
#define LCD_WCD				      0x0053
#define LCD_RGB_INTERFACE   0x00B0
#define LCD_FRC					    0x00B1
#define LCD_BPC					    0x00B5
#define LCD_DFC				 	    0x00B6
#define LCD_POWER1					0x00C0
#define LCD_POWER2					0x00C1
#define LCD_VCOM1						0x00C5
#define LCD_VCOM2						0x00C7
#define LCD_POWERA					0x00CB
#define LCD_POWERB					0x00CF
#define LCD_PGAMMA					0x00E0
#define LCD_NGAMMA					0x00E1
#define LCD_DTCA						0x00E8
#define LCD_DTCB						0x00EA
#define LCD_POWER_SEQ				0x00ED
#define LCD_3GAMMA_EN				0x00F2
#define LCD_INTERFACE				0x00F6
#define LCD_PRC				   	  0x00F7
#define LCD_VERTICAL_SCROLL 0x0033 

#endif /* _SSD1289_H_ */
