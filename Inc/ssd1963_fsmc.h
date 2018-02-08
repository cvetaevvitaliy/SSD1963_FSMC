#ifndef _SSD1963_H_
#define _SSD1963_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_ll_fsmc.h"

#include <stdlib.h>
#include <string.h>

#define BLACK 	0x000000
#define WHITE 	0xFFFFFF
#define RED 		0xFF0000
#define LIME 		0x00FF00
#define BLUE 		0x0000FF
#define BLUE_D 	0x0000A0
#define BLUE_L 	0xADD8E6
#define CYAN 		0x00FFFF
#define YELLOW 	0xFFFF00
#define MAGENTA 0xFF00FF
#define GREEN 	0x008000
#define PURPLE 	0x800080
#define TEAL 		0x008080
#define NAVY 		0x000080
#define SILVER 	0xCOCOCO
#define GRAY 		0x808080
#define ORANGE 	0xFFA500
#define BROWN 	0xA52A2A
#define MAROON 	0x800000
#define OLIVE 	0x808000

uint16_t H24_RGB565(uint32_t color24);
uint16_t RGB_RGB565(uint8_t r, uint8_t g, uint8_t b);



#endif /* _SSD1963_H_ */
