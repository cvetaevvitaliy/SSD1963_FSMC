#ifndef XPT2046_H_
#define XPT2046_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_rcc.h"

#define	XPT2046_SPI 			hspi3
#define	XPT2046_NSS_SOFT	0
#define	XPT2046_NSS_PORT 	GPIOA
#define	XPT2046_NSS_PIN 	GPIO_PIN_15

#define	XPT2046_MIRROR_X 	0
#define	XPT2046_MIRROR_Y 	1

#define	XPT2046_ADDR_I 	0x80
#define	XPT2046_ADDR_X 	0xD0
#define	XPT2046_ADDR_Y 	0x90

#define RAW_MIN_X	200
#define RAW_MAX_X	4000
#define OUT_MIN_X	0
#define OUT_MAX_X	799

#define RAW_MIN_Y	200
#define RAW_MAX_Y	3800
#define OUT_MIN_Y	0
#define OUT_MAX_Y	479

uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);
void XPT2046_Init(void);
uint16_t getRaw(uint8_t address);
uint16_t getX(void);
uint16_t getY(void);

#endif /* XPT2046_H_ */
