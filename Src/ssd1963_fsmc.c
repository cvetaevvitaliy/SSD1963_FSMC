#include "ssd1963_fsmc.h"
#include <stdio.h>
#include "font.h"

char array[100];

uint16_t RGB(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

uint16_t RGB565_Reversed(uint8_t r, uint8_t g, uint8_t b)
{
	return ((b / 8) << 11) | ((g / 4) << 5) | (g / 8);
}

uint16_t H24_RGB565(uint32_t color24)
{
	uint8_t b = (color24 >> 16) & 0xFF;
	uint8_t g = (color24 >> 8) & 0xFF;
	uint8_t r = color24 & 0xFF;
	return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

uint16_t H24_RGB565_Reversed(uint32_t color24)
{
	uint8_t b = (color24 >> 16) & 0xFF;
	uint8_t g = (color24 >> 8) & 0xFF;
	uint8_t r = color24 & 0xFF;
	return ((b / 8) << 11) | ((g / 4) << 5) | (r / 8);
}

void LCD_Data(uint16_t data)
{
	LCD->RAM = data;
}

void LCD_Command(uint16_t instruction)
{
	LCD->REG = instruction;
}

void LCD_Pixel(uint16_t ysta, uint16_t xsta, uint32_t color24)
{
	LCD_CursorPosition(xsta, ysta, xsta, ysta);
	LCD_Data(H24_RGB565(color24));
}

void LCD_CursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_Command(LCD_COLUMN_ADDR);
	LCD_Data(y1 >> 8);
	LCD_Data(y1 & 0xFF);
	LCD_Data(y2 >> 8);
	LCD_Data(y2 & 0xFF);

	LCD_Command(LCD_PAGE_ADDR);
	LCD_Data(x1 >> 8);
	LCD_Data(x1 & 0xFF);
	LCD_Data(x2 >> 8);
	LCD_Data(x2 & 0xFF);
	LCD_Command(LCD_GRAM);
}

void LCD_String_Font(uint16_t x0, uint16_t y0, uint32_t ground24, uint32_t color24, char *s)
{
	unsigned char z, y, nb, he, wi, d, j, q, st1, en1, y2;
	unsigned int x, i, k;
	y2 = y0;
	d = FONT_info[0];
	wi = d * 8;
	he = FONT_info[1];
	nb = (he*d);

	for (z = 0; s[z] != '\0'; z++)
	{
		i = (s[z] - 32)*nb;
		x = i;
		st1 = 0;
		en1 = 0;
		for (q = 0; q < he; q++)
		{
			for (j = 0, k = 0; j < d; j++)
			{
				y = 8;
				while (y--)
				{
					k++;
					if (((FONT[x] & (1 << y)) && (st1 == 0)) || ((FONT[x] & (1 << y)) && (k < st1))) { st1 = k; }
					if ((FONT[x] & (1 << y)) && (en1 < k)) { en1 = k; }
				}
				x++;
			}
		}

		if (st1 > 0) { st1--; }
		if (en1 < 2) { en1 = wi / 3; }
		else if (en1 < wi) { en1 = (en1 - st1 + indent); }

		j = 0;
		k = nb + i;
		q = en1;

		if ((y0 + en1) > LCD_HEIGHT) { y0 = y2; x0++; }
		LCD_CursorPosition(x0*he, y0, (x0*he) + (he - 1), (y0)+(en1 - 1));
		y0 = y0 + en1;

		for (x = i; x < k; x++)
		{
			if (j++ == 0) { y = 10 - st1; }
			else { y = 8; }
			while (y--)
			{
				if ((FONT[x] & (1 << y)) != 0)
				{
					LCD_Data(H24_RGB565_Reversed(color24));
				}

				else
				{
					LCD_Data(H24_RGB565_Reversed(ground24));
				}

				if (!--q)
				{
					if (j != d) { x = x + (d - j); }
					y = 0; j = 0; q = en1;
				}
			}
		}
	}
}

void LCD_Set_X(uint16_t start_x, uint16_t end_x)
{
	LCD_Command(0x002A);
	LCD_Data(start_x >> 8);
	LCD_Data(start_x & 0x00ff);

	LCD_Data(end_x >> 8);
	LCD_Data(end_x & 0x00ff);
}

void LCD_Set_Y(uint16_t start_y, uint16_t end_y)
{
	LCD_Command(0x002B);
	LCD_Data(start_y >> 8);
	LCD_Data(start_y & 0x00ff);

	LCD_Data(end_y >> 8);
	LCD_Data(end_y & 0x00ff);
}

void LCD_Set_XY(uint16_t x, uint16_t y)
{
	LCD_Set_X(x, x);
	LCD_Set_Y(y, y);
}

void LCD_Set_Work_Area(uint16_t x, uint16_t y, uint16_t length, uint16_t width)
{
	LCD_Set_X(x, x + length - 1);
	LCD_Set_Y(y, y + width - 1);
	LCD_Command(0x2C);
}

void LCD_Char(uint16_t x, uint16_t y, uint32_t color24, uint32_t ground24, const unsigned char *table, uint8_t ascii, uint8_t size)
{
	uint8_t i, f = 0;


	for (i = 0; i < 8; i++)
	{
		for (f = 0; f < 8; f++)
		{
			if ((*(table + 8 * (ascii - 0x20) + i) >> (7 - f)) & 0x01)
			{
				LCD_Rectangle_Fill(x + f * size, y + i * size, size, size, color24);
			}
			else
			{
				LCD_Rectangle_Fill(x + f * size, y + i * size, size, size, ground24);
			}
		}
	}
}

void LCD_String(uint16_t x, uint16_t y, uint32_t color24, uint32_t ground24, const unsigned char *table, char *string, uint8_t size)
{
	while (*string)
	{
		if ((x + 8) > MAX_X)
		{
			x = 1;
			y = y + 8 * size;
		}
		LCD_Char(x, y, color24, ground24, table, *string, size);
		x += 8 * size;
		string++;
	}
}

void LCD_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color24, uint8_t size)
{
	int deltaX = abs(x2 - x1);
	int deltaY = abs(y2 - y1);
	int signX = x1 < x2 ? 1 : -1;
	int signY = y1 < y2 ? 1 : -1;
	int error = deltaX - deltaY;
	int error2 = 0;

	for (;;)
	{
		LCD_Rectangle_Fill(x1, y1, size, size, color24);

		if (x1 == x2 && y1 == y2)
			break;

		error2 = error * 2;

		if (error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}

		if (error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
	}
}

void LCD_HLine(uint16_t x, uint16_t y, uint16_t length, uint16_t size, uint32_t color24)
{
	uint16_t i = 0;
	LCD_Set_Work_Area(x, y, length, size);
	for (i = 0; i < (length*size); i++)
		LCD_Data(H24_RGB565_Reversed(color24));
}

void LCD_VLine(uint16_t x, uint16_t y, uint16_t length, uint16_t size, uint32_t color24)
{
	uint16_t i = 0;
	LCD_Set_Work_Area(x, y, size, length);
	for (i = 0; i < (length*size); i++)
		LCD_Data(H24_RGB565_Reversed(color24));
}

void LCD_Rectangle(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint8_t size, uint32_t color24)
{
	LCD_HLine(x, y, length, size, color24);
	LCD_HLine(x, y + width, length, size, color24);
	LCD_VLine(x, y, width, size, color24);
	LCD_VLine(x + length - size, y, width, size, color24);
}

void LCD_Rectangle_Fill(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint32_t color24)
{
	uint32_t i = 0;

	LCD_Set_Work_Area(x, y, length, width);
	for (i = 0; i < length*width; i++)
	{
		LCD_Data(H24_RGB565_Reversed(color24));
	}
}

void LCD_Triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t size, uint32_t color24)
{

	LCD_Line(x1, y1, x2, y2, color24, size);
	LCD_Line(x2, y2, x3, y3, color24, size);
	LCD_Line(x3, y3, x1, y1, color24, size);
}

void LCD_Circle(uint16_t x, uint16_t y, uint8_t radius, uint8_t fill, uint8_t size, uint32_t color24)
{
	int a_, b_, P;
	a_ = 0;
	b_ = radius;
	P = 1 - radius;
	while (a_ <= b_)
	{
		if (fill == 1)
		{
			LCD_Rectangle_Fill(x - a_, y - b_, 2 * a_ + 1, 2 * b_ + 1, color24);
			LCD_Rectangle_Fill(x - b_, y - a_, 2 * b_ + 1, 2 * a_ + 1, color24);
		}
		else
		{
			LCD_Rectangle_Fill(a_ + x, b_ + y, size, size, color24);
			LCD_Rectangle_Fill(b_ + x, a_ + y, size, size, color24);
			LCD_Rectangle_Fill(x - a_, b_ + y, size, size, color24);
			LCD_Rectangle_Fill(x - b_, a_ + y, size, size, color24);
			LCD_Rectangle_Fill(b_ + x, y - a_, size, size, color24);
			LCD_Rectangle_Fill(a_ + x, y - b_, size, size, color24);
			LCD_Rectangle_Fill(x - a_, y - b_, size, size, color24);
			LCD_Rectangle_Fill(x - b_, y - a_, size, size, color24);
		}
		if (P < 0)
		{
			P = (P + 3) + (2 * a_);
			a_++;
		}
		else
		{
			P = (P + 5) + (2 * (a_ - b_));
			a_++;
			b_--;
		}
	}
}

void LCD_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint8_t size, uint32_t color24)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4) {
			LCD_Rectangle_Fill(x0 + x, y0 + y, size, size, color24);
			LCD_Rectangle_Fill(x0 + y, y0 + x, size, size, color24);
		}
		if (cornername & 0x2) {
			LCD_Rectangle_Fill(x0 + x, y0 - y, size, size, color24);
			LCD_Rectangle_Fill(x0 + y, y0 - x, size, size, color24);
		}
		if (cornername & 0x8) {
			LCD_Rectangle_Fill(x0 - y, y0 + x, size, size, color24);
			LCD_Rectangle_Fill(x0 - x, y0 + y, size, size, color24);
		}
		if (cornername & 0x1) {
			LCD_Rectangle_Fill(x0 - y, y0 - x, size, size, color24);
			LCD_Rectangle_Fill(x0 - x, y0 - y, size, size, color24);
		}
	}
}

void LCD_Round_Rect(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint8_t size, uint32_t color24)
{

	LCD_HLine(x + r, y, length - 2 * r, size, color24);
	LCD_HLine(x + r, y + width - 1, length - 2 * r, size, color24);
	LCD_VLine(x, y + r, width - 2 * r, size, color24);
	LCD_VLine(x + length - 1, y + r, width - 2 * r, size, color24);

	LCD_Circle_Helper(x + r, y + r, r, 1, size, color24);
	LCD_Circle_Helper(x + length - r - 1, y + r, r, 2, size, color24);
	LCD_Circle_Helper(x + length - r - 1, y + width - r - 1, r, 4, size, color24);
	LCD_Circle_Helper(x + r, y + width - r - 1, r, 8, size, color24);
}

void LCD_Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint32_t color24)
{

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1) {
			LCD_VLine(x0 + x, y0 - y, 2 * y + 1 + delta, 1, color24);
			LCD_VLine(x0 + y, y0 - x, 2 * x + 1 + delta, 1, color24);
		}
		if (cornername & 0x2) {
			LCD_VLine(x0 - x, y0 - y, 2 * y + 1 + delta, 1, color24);
			LCD_VLine(x0 - y, y0 - x, 2 * x + 1 + delta, 1, color24);
		}
	}
}

void LCD_Round_Rect_Fill(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint32_t color24)
{
	LCD_Rectangle_Fill(x + r, y, length - 2 * r, width, color24);
	LCD_Fill_Circle_Helper(x + length - r - 1, y + r, r, 1, width - 2 * r - 1, color24);
	LCD_Fill_Circle_Helper(x + r, y + r, r, 2, width - 2 * r - 1, color24);
}

const unsigned char font8x8[][8] = {
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},// space
	{0x04,0x04,0x04,0x04,0x00,0x04,0x00,0x00},// !
	{0x0A,0x0A,0x0A,0x00,0x00,0x00,0x00,0x00},// "
	{0x0A,0x0A,0x1F,0x0A,0x1F,0x0A,0x0A,0x00},// #
	{0x04,0x0F,0x14,0x0E,0x05,0x1E,0x04,0x00},// $
	{0x18,0x19,0x02,0x04,0x08,0x13,0x03,0x00},// %
	{0x0C,0x12,0x14,0x08,0x14,0x12,0x0D,0x00},// &
	{0x0C,0x04,0x08,0x00,0x00,0x00,0x00,0x00},// '
	{0x02,0x04,0x08,0x08,0x08,0x04,0x02,0x00},// (
	{0x08,0x04,0x02,0x02,0x02,0x04,0x08,0x00},// )
	{0x00,0x04,0x15,0x0E,0x15,0x04,0x00,0x00},// *
	{0x00,0x04,0x04,0x1F,0x04,0x04,0x00,0x00},// +
	{0x00,0x00,0x00,0x00,0x0C,0x04,0x08,0x00},// ,
	{0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00},// -
	{0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00},// .
	{0x00,0x01,0x02,0x04,0x08,0x10,0x00,0x00},// /

	{0x0E,0x11,0x13,0x15,0x19,0x11,0x0E,0x00},// 0
	{0x04,0x0C,0x04,0x04,0x04,0x04,0x0E,0x00},// 1
	{0x0E,0x11,0x01,0x02,0x04,0x08,0x1F,0x00},// 2
	{0x1F,0x02,0x04,0x02,0x01,0x11,0x0E,0x00},// 3
	{0x02,0x06,0x0A,0x12,0x1F,0x02,0x02,0x00},// 4
	{0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E,0x00},// 5
	{0x06,0x08,0x10,0x1E,0x11,0x11,0x0E,0x00},// 6
	{0x1F,0x01,0x02,0x04,0x08,0x08,0x08,0x00},// 7
	{0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E,0x00},// 8
	{0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C,0x00},// 9
	{0x00,0x01,0x02,0x04,0x08,0x10,0x00,0x00},// :
	{0x00,0x0C,0x0C,0x00,0x0C,0x04,0x08,0x00},// ;
	{0x02,0x04,0x08,0x10,0x08,0x04,0x02,0x00},// <
	{0x00,0x00,0x1F,0x00,0x1F,0x00,0x00,0x00},// =
	{0x08,0x04,0x02,0x01,0x02,0x04,0x08,0x00},// >
	{0x0E,0x11,0x01,0x02,0x04,0x00,0x04,0x00},// ?

	{0x0E,0x11,0x01,0x0D,0x15,0x15,0x0E,0x00},// @
	{0x0E,0x11,0x11,0x11,0x1F,0x11,0x11,0x00},// A
	{0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E,0x00},// B
	{0x0E,0x11,0x10,0x10,0x10,0x11,0x0E,0x00},// C
	{0x1C,0x12,0x11,0x11,0x11,0x12,0x1C,0x00},// D
	{0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F,0x00},// E
	{0x1F,0x10,0x10,0x1E,0x10,0x10,0x10,0x00},// F
	{0x0E,0x11,0x10,0x17,0x11,0x11,0x0E,0x00},// G
	{0x11,0x11,0x11,0x1F,0x11,0x11,0x11,0x00},// H
	{0x0E,0x04,0x04,0x04,0x04,0x04,0x0E,0x00},// I
	{0x07,0x02,0x02,0x02,0x02,0x12,0x0C,0x00},// J
	{0x11,0x12,0x14,0x18,0x14,0x12,0x11,0x00},// K
	{0x10,0x10,0x10,0x10,0x10,0x10,0x1F,0x00},// L
	{0x11,0x1B,0x15,0x15,0x11,0x11,0x11,0x00},// M
	{0x11,0x11,0x19,0x15,0x13,0x11,0x11,0x00},// N
	{0x0E,0x11,0x11,0x11,0x11,0x11,0x0E,0x00},// O

	{0x1E,0x11,0x11,0x1E,0x10,0x10,0x10,0x00},// P
	{0x0E,0x11,0x11,0x11,0x15,0x12,0x0D,0x00},// Q
	{0x1E,0x11,0x11,0x1E,0x14,0x12,0x11,0x00},// R
	{0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E,0x00},// S
	{0x1F,0x04,0x04,0x04,0x04,0x04,0x04,0x00},// T
	{0x11,0x11,0x11,0x11,0x11,0x11,0x0E,0x00},// U
	{0x11,0x11,0x11,0x11,0x11,0x0A,0x04,0x00},// V
	{0x11,0x11,0x11,0x11,0x15,0x15,0x0E,0x00},// W
	{0x11,0x11,0x0A,0x04,0x0A,0x11,0x11,0x00},// X
	{0x11,0x11,0x11,0x0A,0x04,0x04,0x04,0x00},// Y
	{0x1F,0x01,0x02,0x04,0x08,0x10,0x1F,0x00},// Z
	{0x0E,0x08,0x08,0x08,0x08,0x08,0x0E,0x00},// [
	{0x11,0x0A,0x1F,0x04,0x1F,0x04,0x04,0x00},//
	{0x0E,0x02,0x02,0x02,0x02,0x02,0x0E,0x00},// ]
	{0x04,0x0A,0x11,0x00,0x00,0x00,0x00,0x00},// ^
	{0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x00},// _

	{0x08,0x04,0x00,0x00,0x00,0x00,0x00,0x00},// '
	{0x00,0x00,0x0E,0x01,0x0F,0x11,0x0F,0x00},// a
	{0x10,0x10,0x1E,0x11,0x11,0x11,0x1E,0x00},// b
	{0x00,0x00,0x0E,0x10,0x10,0x11,0x0E,0x00},// c
	{0x01,0x01,0x0D,0x13,0x11,0x11,0x0F,0x00},// d
	{0x00,0x00,0x0E,0x11,0x1F,0x10,0x0E,0x00},// e
	{0x06,0x09,0x08,0x1C,0x08,0x08,0x08,0x00},// f
	{0x00,0x0F,0x11,0x11,0x0F,0x01,0x0E,0x00},// g
	{0x10,0x10,0x16,0x19,0x11,0x11,0x11,0x00},// h
	{0x04,0x00,0x0C,0x04,0x04,0x04,0x0E,0x00},// i
	{0x02,0x00,0x06,0x02,0x02,0x12,0x0C,0x00},// j
	{0x10,0x10,0x12,0x14,0x18,0x14,0x12,0x00},// k
	{0x18,0x08,0x08,0x08,0x08,0x08,0x1C,0x00},// l
	{0x00,0x00,0x1A,0x15,0x15,0x11,0x11,0x00},// m
	{0x00,0x00,0x16,0x19,0x11,0x11,0x11,0x00},// n
	{0x00,0x00,0x0E,0x11,0x11,0x11,0x0E,0x00},// o

	{0x00,0x00,0x1E,0x11,0x1E,0x10,0x10,0x00},// p
	{0x00,0x00,0x0F,0x11,0x0F,0x01,0x01,0x00},// q
	{0x00,0x00,0x16,0x19,0x10,0x10,0x10,0x00},// r
	{0x00,0x00,0x0E,0x10,0x0E,0x01,0x1E,0x00},// s
	{0x08,0x08,0x1C,0x08,0x08,0x09,0x06,0x00},// t
	{0x00,0x00,0x11,0x11,0x11,0x13,0x0D,0x00},// u
	{0x00,0x00,0x11,0x11,0x11,0x0A,0x04,0x00},// v
	{0x00,0x00,0x11,0x11,0x11,0x15,0x0A,0x00},// w
	{0x00,0x00,0x11,0x0A,0x04,0x0A,0x11,0x00},// x
	{0x00,0x00,0x11,0x11,0x0F,0x01,0x0E,0x00},// y
	{0x00,0x00,0x1F,0x02,0x04,0x08,0x1F,0x00},// z
	{0x17,0x15,0x15,0x15,0x17,0x00,0x00,0x00},// 10
	{0x17,0x11,0x17,0x14,0x17,0x00,0x00,0x00},// 12
	{0x17,0x14,0x17,0x11,0x17,0x00,0x00,0x00},// 15
	{0x01,0x05,0x09,0x1F,0x08,0x04,0x00,0x00},// ñòðåëêà
	{0x00,0x07,0x0E,0x1C,0x11,0x1E,0x00,0x00} };// êðàêîçÿáëèê

	void SSD1963_Init(void)
	{
		//1. Power up the system platform and assert the RESET# signal (‘L’ state) for a minimum of 100us to reset the controller. 
		//		LCD_RST_SET
		//    HAL_Delay (100);
		//    LCD_RST_RESET
		//    HAL_Delay (120);
		//    LCD_RST_SET
		HAL_Delay(150);
		/*	2. Configure SSD1961’s PLL frequency
		VCO = Input clock x (M + 1)
		PLL frequency  = VCO / (N + 1)
		* Note :
		1.  250MHz < VCO < 800MHz
		PLL frequency < 110MHz
		2.  For a 10MHz input clock to obtain 100MHz PLL frequency, user cannot program M = 19 and N = 1.  The
		closet setting in this situation is setting M=29 and N=2, where 10 x 30 / 3 = 100MHz.
		3.  Before PLL is locked, SSD1961/2/3 is operating at input clock frequency (e.g. 10MHz), registers
		programming cannot be set faster than half of the input clock frequency (5M words/s in this example).
		Example to program SSD1961 with M = 29, N = 2, VCO = 10M x 30 = 300 MHz, PLL frequency = 300M / 3 = 100
		MHz
		******************************/
		LCD_Command(LCD_RESET);
		HAL_Delay(150);
		
		LCD_Command(0xE2);//set frequency
		LCD_Data(0x1D);  // presceller(M=29) 
		LCD_Data(0x02);  //multiplier(N=2) 
		LCD_Data(0xFF);  //on-off multiplier and presceller
		//3. Turn on the PLL 
		LCD_Command(0xE0);
		LCD_Data(0x01);
		HAL_Delay(120); // Wait for 100us to let the PLL stable and read the PLL lock status bit. 
		LCD_Command(0xE0);
		//READ COMMAND “0xE4);   (Bit 2 = 1 if PLL locked) 
		LCD_Data(0x03); // 5. Switch the clock source to PLL 
		HAL_Delay(120);
		LCD_Command(0x01); //6. Software Reset
		HAL_Delay(120);
		/*************
		Dot clock Freq = PLL Freq x (LCDC_FPR + 1) / 2^20
		For example,  22MHz = 100MHz * (LCDC_FPR+1) / 2^20
		LCDC_FPR = 230685 = 0x3851D
		********************/
		LCD_Command(0xE6);  // 7. Configure the dot clock frequency
		LCD_Data(0x03);
		LCD_Data(0x85);
		LCD_Data(0x1D);
		//8. Configure the LCD panel  
		//a. Set the panel size to 480 x 800 and polarity of LSHIFT, LLINE and LFRAME to active low 
		LCD_Command(0xB0);
		if (LSHIFT) LCD_Data(0x0C); /* 0x08 0x0C 0xAE(5') */else LCD_Data(0xAE); //18bit panel, disable dithering, LSHIFT: Data latch in rising edge, LLINE and LFRAME: active low 
		LCD_Data(0x20);  /* 0x00 0x80 0x20(5') */    // TFT type 
		LCD_Data(0x03);     // Horizontal Width:  480 - 1 = 0x031F 
		LCD_Data(0x1F);
		LCD_Data(0x01);     // Vertical Width :  800 -1 = 0x01DF
		LCD_Data(0xDF);
		LCD_Data(0x2D);  /* 0x00 0x2d */   // 000 = режим RGB
											   //b. Set the horizontal period 
		LCD_Command(0xB4); // Horizontal Display Period  
		LCD_Data(0x03);    // HT: horizontal total period (display + non-display) – 1 = 520-1 =  519 =0x0207
		LCD_Data(0xA0);
		LCD_Data(0x00);    // HPS: Horizontal Sync Pulse Start Position = Horizontal Pulse Width + Horizontal Back Porch = 16 = 0x10 
		LCD_Data(0x2E);
		LCD_Data(0x30);     // HPW: Horizontal Sync Pulse Width - 1=8-1=7 
		LCD_Data(0x00);    // LPS: Horizontal Display Period Start Position = 0x0000 
		LCD_Data(0x0F);
		LCD_Data(0x00);    // LPSPP: Horizontal Sync Pulse Subpixel Start Position(for serial TFT interface).  Dummy value for TFT interface. 
							   //c. Set the vertical period 
		LCD_Command(0xB6);    // Vertical Display Period  
		LCD_Data(0x02);     // VT: Vertical Total (display + non-display) Period – 1  =647=0x287 
		LCD_Data(0x0D);
		LCD_Data(0x00);     // VPS: Vertical Sync Pulse Start Position  =     Vertical Pulse Width + Vertical Back Porch = 2+2=4  
		LCD_Data(0x10);
		LCD_Data(0x10);     //VPW: Vertical Sync Pulse Width – 1 =1 
		LCD_Data(0x08);     //FPS: Vertical Display Period Start Position = 0 
		LCD_Data(0x00);
		//9. Set the back light control PWM clock frequency
		//PWM signal frequency = PLL clock / (256 * (PWMF[7:0] + 1)) / 256
		LCD_Command(0xBE);    // PWM configuration 
		LCD_Data(0x08);     // set PWM signal frequency to 170Hz when PLL frequency is 100MHz 
		LCD_Data(0xFF);     // PWM duty cycle  (50%) 
		LCD_Data(0x01);     // 0x09 = enable DBC, 0x01 = disable DBC  //включаем
		LCD_Command(0x36);     // set address_mode
		if (MIRROR_H) LCD_Data(0x02); else if (MIRROR_V) LCD_Data(0x03);
		//13. Setup the MCU interface for 16-bit data write (565 RGB)
		LCD_Command(0xF0);     // mcu interface config 
		LCD_Data(0x03);     // 16 bit interface (565)
								//10. Turn on the display 
		LCD_Command(LCD_DISPLAY_ON);     // display on 
	}

	void SSD1963_Bright(uint8_t bright)
	{
		LCD_Command(0xBE);  // PWM configuration 
		LCD_Data(0x08);     // set PWM signal frequency to 170Hz when PLL frequency is 100MHz 
		LCD_Data(bright);   // PWM duty cycle  
		LCD_Data(0x01);
	}
	
	void SSD1963_Test(void)
	{
	LCD_Rectangle_Fill(0, 0, 800, 480, RED);	
	HAL_Delay(500);
	LCD_Rectangle_Fill(0, 0, 800, 480, GREEN);
	HAL_Delay(500);
	LCD_Rectangle_Fill(0, 0, 800, 480, BLUE);
	HAL_Delay(500);
	LCD_Rectangle_Fill(0, 0, 800, 480, MAGENTA);
	LCD_Rectangle_Fill(1, 1, 798, 478, BLACK);
	sprintf(array, "Test Test Test");
	LCD_String_Font(1, 5, BLUE, RED, array);
	sprintf(array, "TEST");
	LCD_String_Font(0, 610, BLACK, RED, array);
	LCD_String(50, 100, GREEN, RED, *font8x8, "Calibration", 3);	
	LCD_Line(LCD_PIXEL_WIDTH -10-50, 10+25, LCD_PIXEL_WIDTH -10-50+50, 10+25, WHITE, 1);
	LCD_Line(LCD_PIXEL_WIDTH -10-25, 10, LCD_PIXEL_WIDTH -10-25, 10+50, WHITE, 1);
	LCD_Char(500, 100, RED, BLACK, *font8x8,'R',3);
	LCD_Char(500, 200, GREEN, BLACK, *font8x8,'G',3);
	LCD_Char(200, 300, BLUE, BLACK, *font8x8,'B',3);
	LCD_String(400, 300, BLUE, BLACK, *font8x8, "TEST",5);
	LCD_Rectangle(600, 300, 100, 100, 4, BLACK);
	LCD_Line(120, 120, 280, 280, MAGENTA, 3);
	LCD_Triangle(100, 100, 100, 200, 300, 300, 2, BLUE);
	LCD_Circle(400, 400, 50, 0, 2, YELLOW);
	LCD_Circle(400, 400, 50, 1, 2, YELLOW);
	LCD_Round_Rect(150, 350, 100, 100, 10, 2, WHITE);
	LCD_Round_Rect_Fill(400, 150, 100, 100, 10, WHITE);
	LCD_Round_Rect_Fill(150, 50, 170, 50, 10, WHITE);
	LCD_String(170, 65, BLUE, WHITE, *font8x8, "Start", 3);
	}
