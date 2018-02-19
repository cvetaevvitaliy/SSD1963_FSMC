#ifndef _SSD1289_H_
#define _SSD1289_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_ll_fsmc.h"

#include <stdlib.h>
#include <string.h>

/* 8x8 */
extern const unsigned char tiny_8x8[];
extern const unsigned char sinclair_8x8[];
extern const unsigned char sinclair_8x8_inverted[];

/* 8x9 */
extern const unsigned char myke_8x9[];

/* 8x12 */
extern const unsigned char default_8x12[];

/* 8x16 */
extern const unsigned char old_8x16[];

/* 16x16 */
extern const unsigned char default_16x16[];
extern const unsigned char arial_16x16_bold[];
extern const unsigned char arial_16x16_italic[];
extern const unsigned char arial_16x16_normal[];
extern const unsigned char franklingothic_16x16[];
extern const unsigned char hallfetica_16x16[];
extern const unsigned char nadianne_16x16[];
extern const unsigned char sinclair_16x16[];
extern const unsigned char sinclair_16x16_inverted[];
extern const unsigned char swiss_16x16[];

/* 16x22 */
extern const unsigned char matrix_16x22[];
extern const unsigned char matrix_16x22_slash[];

/* 16x24 */
extern const unsigned char arial_16x24_round[];
extern const unsigned char ocr_16x24[];

/* 16x32 */
extern const unsigned char old_16x32[];

/* 24x32 */
extern const unsigned char inconsola_24x32[];
extern const unsigned char ubuntu_24x32[];
extern const unsigned char ubuntu_24x32_bold[];

/* NUMERIC */
extern const unsigned char arial_32x50_num[];
extern const unsigned char calibri_32x48_bold_num[];
extern const unsigned char matrix_24x29_num[];
extern const unsigned char matrix_32x50_num[];
extern const unsigned char segment_seven_32x50_num[];
extern const unsigned char segment_seven_64x100_num[];
extern const unsigned char segment_seven_96x144_num[];
extern const unsigned char segment_sixteen_48x72_num[];
extern const unsigned char segment_sixteen_64x96_num[];
extern const unsigned char segment_sixteen_96x144_num[];
extern const unsigned char segment_sixteen_128x192_num[];

/* SEGMENTED */
extern const unsigned char segment_eighteen_32x52[];
extern const unsigned char segment_sixteen_16x24[];
extern const unsigned char segment_sixteen_24x36[];
extern const unsigned char segment_sixteen_32x48[];
extern const unsigned char segment_sixteen_32x50[];
extern const unsigned char segment_sixteen_40x60[];

/* SYMBOLS */
extern const unsigned char battery_24x48[];
extern const unsigned char barcode_rm4scc_16x16[];
extern const unsigned char barcode_post_40x20_num[];
extern const unsigned char dingbats1_32x24[];
extern const unsigned char symbols_16x16[];
extern const unsigned char symbols_16x32_1[];
extern const unsigned char symbols_16x32_2[];
extern const unsigned char symbols_32x32[];

#define CMD *(uint16_t *)0x60000000
#define DAT *(uint16_t *)0x60020000

#define MIRROR_V 0
#define MIRROR_H 0

#define LCD_RST_SET   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
#define LCD_RST_RESET HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
											
#define LSHIFT 0

#define LCD_WIDTH       480
#define LCD_HEIGHT      800

uint16_t RGB(uint8_t r, uint8_t g, uint8_t b);

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

void SSD1963_Init(uint8_t bright);
void SSD1963_Bright(uint8_t bright);
void SSD1963_Test(void);

void LCD_Pixel(uint16_t ysta, uint16_t xsta, uint32_t color24);
void LCD_String(uint16_t x0, uint16_t y0, uint32_t color24, uint32_t ground24, const unsigned char *font, char *s);
void LCD_Char_Scale_1B(uint16_t x, uint16_t y, uint32_t color24, uint32_t ground24, const unsigned char *font, uint8_t ascii, uint8_t size);
void LCD_String_Scale_1B(uint16_t x, uint16_t y, uint32_t color24, uint32_t ground24, const unsigned char *font, char *string, uint8_t size);
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

#ifdef __cplusplus
}
#endif

#endif /* _SSD1289_H_ */
