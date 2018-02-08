#include "ssd1963_fsmc.h"

uint16_t HDP, VDP;

uint16_t H24_RGB565(uint32_t color24)
{
uint8_t b = (color24 >> 16) & 0xFF;
uint8_t g = (color24 >> 8) & 0xFF;
uint8_t r = color24 & 0xFF;
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

uint16_t RGB_RGB565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

