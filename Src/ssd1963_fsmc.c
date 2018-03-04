#include "ssd1963_fsmc.h"

uint16_t RGB(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

static inline uint16_t H24_RGB565(uint32_t color24)
{
	uint8_t b = (color24 >> 16) & 0xFF;
	uint8_t g = (color24 >> 8) & 0xFF;
	uint8_t r = color24 & 0xFF;
	return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

static inline uint16_t H24_RGB565_Reversed(uint32_t color24)
{
	uint8_t b = (color24 >> 16) & 0xFF;
	uint8_t g = (color24 >> 8) & 0xFF;
	uint8_t r = color24 & 0xFF;
	return ((b / 8) << 11) | ((g / 4) << 5) | (r / 8);
}

static inline void LCD_Send_Cmd(uint16_t cmd)
{
	CMD= cmd;
}

static inline void LCD_Send_Dat(uint16_t dat)
{
	DAT = dat;
}

static inline void LCD_Set_X(uint16_t start_x, uint16_t end_x)
{
	LCD_Send_Cmd(LCD_COLUMN_ADDR);
	LCD_Send_Dat(start_x >> 8);
	LCD_Send_Dat(start_x & 0x00FF);

	LCD_Send_Dat(end_x >> 8);
	LCD_Send_Dat(end_x & 0x00FF);
}

static inline void LCD_Set_Y(uint16_t start_y, uint16_t end_y)
{
	LCD_Send_Cmd(LCD_PAGE_ADDR);
	LCD_Send_Dat(start_y >> 8);
	LCD_Send_Dat(start_y & 0x00FF);

	LCD_Send_Dat(end_y >> 8);
	LCD_Send_Dat(end_y & 0x00FF);
}

static inline void LCD_Position(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_Set_X(y1, y2);
	LCD_Set_Y(x1, x2);
	LCD_Send_Cmd(LCD_GRAM);
}

void LCD_Pixel(uint16_t ysta, uint16_t xsta, uint32_t color24)
{
	LCD_Position(xsta, ysta, xsta, ysta);
	LCD_Send_Dat(H24_RGB565(color24));
}

void LCD_String(uint16_t x0, uint16_t y0, uint32_t color24, uint32_t ground24, const unsigned char *font, char *s)
{
	unsigned char infoblock =  4;
	unsigned char lineblock = font[0];
//	unsigned char width 		= font[1];
	unsigned char height 		= font[2];
	unsigned char startchar = font[3];
	
	uint8_t indent = 3;
	
	unsigned char z = 0, y, j, q, st1, en1, y2;
	unsigned int x, i, k;
	y2 = y0;
	
	for (z = 0; s[z] != '\0'; z++)
	{
		i = (s[z] - startchar) * (height * lineblock);
		x = i;
		st1 = 0;
		en1 = 0;
		for (q = 0; q < height; q++)
		{
			for (j = 0, k = 0; j < lineblock; j++)
			{
				y = 8;
				while (y--)
				{
					k++;
					if (((font[x + infoblock] & (1 << y)) && (st1 == 0)) || ((font[x + infoblock] & (1 << y)) && (k < st1))) { st1 = k; }
					if ((font[x + infoblock] & (1 << y)) && (en1 < k)) { en1 = k; }
				}
				x++;
			}
		}

		if (st1 > 0) { st1--; }
		if (en1 < 2) { en1 =  (lineblock * 8) / 3; }
		else if (en1 < (lineblock * 8)) { en1 = (en1 - st1 + indent); }

		j = 0;
		k = (height * lineblock) + i;
		q = en1;

		if ((y0 + en1) > LCD_HEIGHT) { y0 = y2; x0++; }
		LCD_Position(x0, y0, x0 + (height - 1), (y0) + (en1 - 1));
		y0 = y0 + en1;

		for (x = i; x < k; x++)
		{
			if (j++ == 0) { y = 10 - st1; }
			else { y = 8; }
			while (y--)
			{
				if ((font[x + infoblock] & (1 << y)) != 0)
				{
					LCD_Send_Dat(H24_RGB565_Reversed(color24));
				}

				else
				{
					LCD_Send_Dat(H24_RGB565_Reversed(ground24));
				}

				if (!--q)
				{
					if (j != lineblock) { x = x + (lineblock - j); }
					y = 0; j = 0; q = en1;
				}
			}
		}
	}
}

void LCD_Char_Scale_1B(uint16_t x, uint16_t y, uint32_t color24, uint32_t ground24, const unsigned char *font, uint8_t ascii, uint8_t size)
{
	uint8_t fontinfo  =  3;
	
//	unsigned char lineblock = font[0];	
	unsigned char width  		= font[1];
	unsigned char height 		= font[2];
	unsigned char startchar = font[3];
	
	uint8_t i, f = 0;
	
	for (i = 0; i < height; i++)
	{
		for (f = 0; f < width; f++)
		{
			if ((*(font + fontinfo + height * (ascii - startchar) + i) >> (7 - f)) & 0x01)
			{
				LCD_Rect_Fill(x + f * size, y + i * size, size, size, color24);
			}
			else
			{
				LCD_Rect_Fill(x + f * size, y + i * size, size, size, ground24);
			}
		}
	}
}

void LCD_String_Scale_1B(uint16_t x, uint16_t y, uint32_t color24, uint32_t ground24, const unsigned char *font, char *string, uint8_t size)
{
	while (*string)
	{
		if ((x + 8) > LCD_HEIGHT - 1)
		{
			x = 1;
			y = y + 8 * size;
		}
		LCD_Char_Scale_1B(x, y, color24, ground24, font, *string, size);
		x += 8 * size;
		string++;
	}
}

void LCD_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint32_t color24)
{
	int deltaX = abs(x2 - x1);
	int deltaY = abs(y2 - y1);
	int signX = x1 < x2 ? 1 : -1;
	int signY = y1 < y2 ? 1 : -1;
	int error = deltaX - deltaY;
	int error2 = 0;

	for (;;)
	{
		LCD_Rect_Fill(x1, y1, size, size, color24);

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

static inline void LCD_Line_H(uint16_t x, uint16_t y, uint16_t length, uint16_t size, uint32_t color24)
{
	LCD_Line(x, y, x + length - size, y, size, color24);
}

static inline void LCD_Line_V(uint16_t x, uint16_t y, uint16_t length, uint16_t size, uint32_t color24)
{
	LCD_Line(x, y, x, y + length - size, size, color24);
}

void LCD_Rect(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint8_t size, uint32_t color24)
{
	LCD_Line_H(x, y, length, size, color24);
	LCD_Line_H(x, y + width, length, size, color24);
	LCD_Line_V(x, y, width, size, color24);
	LCD_Line_V(x + length - size, y, width, size, color24);
}

void LCD_Rect_Fill(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint32_t color24)
{
	uint32_t i = 0;

	LCD_Position(y, x, y + width - 1, x + length - 1);
	for (i = 0; i < length * width; i++)
	{
		LCD_Send_Dat(H24_RGB565_Reversed(color24));
	}
}

void LCD_Triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t size, uint32_t color24)
{
	LCD_Line(x1, y1, x2, y2, size, color24);
	LCD_Line(x2, y2, x3, y3, size, color24);
	LCD_Line(x3, y3, x1, y1, size, color24);
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
		LCD_Rect_Fill(x - a_, y - b_, 2 * a_ + 1, 2 * b_ + 1, color24);
		LCD_Rect_Fill(x - b_, y - a_, 2 * b_ + 1, 2 * a_ + 1, color24);
		}
		else
		{
		LCD_Rect_Fill(a_ + x, b_ + y, size, size, color24);
		LCD_Rect_Fill(b_ + x, a_ + y, size, size, color24);
		LCD_Rect_Fill(x - a_, b_ + y, size, size, color24);
		LCD_Rect_Fill(x - b_, a_ + y, size, size, color24);
		LCD_Rect_Fill(b_ + x, y - a_, size, size, color24);
		LCD_Rect_Fill(a_ + x, y - b_, size, size, color24);
		LCD_Rect_Fill(x - a_, y - b_, size, size, color24);
		LCD_Rect_Fill(x - b_, y - a_, size, size, color24);
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
			LCD_Rect_Fill(x0 + x, y0 + y, size, size, color24);
			LCD_Rect_Fill(x0 + y, y0 + x, size, size, color24);
		}
		if (cornername & 0x2) {
			LCD_Rect_Fill(x0 + x, y0 - y, size, size, color24);
			LCD_Rect_Fill(x0 + y, y0 - x, size, size, color24);
		}
		if (cornername & 0x8) {
			LCD_Rect_Fill(x0 - y, y0 + x, size, size, color24);
			LCD_Rect_Fill(x0 - x, y0 + y, size, size, color24);
		}
		if (cornername & 0x1) {
			LCD_Rect_Fill(x0 - y, y0 - x, size, size, color24);
			LCD_Rect_Fill(x0 - x, y0 - y, size, size, color24);
		}
	}
}

void LCD_Rect_Round(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint8_t size, uint32_t color24)
{
	LCD_Line_H(x + r, y, length - 2 * r, size, color24);
	LCD_Line_H(x + r, y + width - 1, length - 2 * r, size, color24);
	LCD_Line_V(x, y + r, width - 2 * r, size, color24);
	LCD_Line_V(x + length - 1, y + r, width - 2 * r, size, color24);

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
			LCD_Line_V(x0 + x, y0 - y, 2 * y + 1 + delta, 1, color24);
			LCD_Line_V(x0 + y, y0 - x, 2 * x + 1 + delta, 1, color24);
		}
		if (cornername & 0x2) {
			LCD_Line_V(x0 - x, y0 - y, 2 * y + 1 + delta, 1, color24);
			LCD_Line_V(x0 - y, y0 - x, 2 * x + 1 + delta, 1, color24);
		}
	}
}

void LCD_Rect_Round_Fill(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint16_t r, uint32_t color24)
{
	LCD_Rect_Fill(x + r, y, length - 2 * r, width, color24);
	LCD_Fill_Circle_Helper(x + length - r - 1, y + r, r, 1, width - 2 * r - 1, color24);
	LCD_Fill_Circle_Helper(x + r, y + r, r, 2, width - 2 * r - 1, color24);
}

void SSD1963_Init(uint8_t bright)
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
	LCD_Send_Cmd(LCD_RESET);
	LCD_Send_Cmd(LCD_DISPLAY_ON); 		
	LCD_Send_Cmd(0xE2);//set frequency
	LCD_Send_Dat(0x1D);  // presceller(M=29) 
	LCD_Send_Dat(0x02);  //multiplier(N=2) 
	LCD_Send_Dat(0xFF);  //on-off multiplier and presceller
	//3. Turn on the PLL 
	LCD_Send_Cmd(0xE0);
	LCD_Send_Dat(0x01);
	HAL_Delay(120); // Wait for 100us to let the PLL stable and read the PLL lock status bit. 
	LCD_Send_Cmd(0xE0);
	//READ COMMAND “0xE4);   (Bit 2 = 1 if PLL locked) 
	LCD_Send_Dat(0x03); // 5. Switch the clock source to PLL 
	HAL_Delay(120);
	LCD_Send_Cmd(0x01); //6. Software Reset
	HAL_Delay(120);
	/*************
	Dot clock Freq = PLL Freq x (LCDC_FPR + 1) / 2^20
	For example,  22MHz = 100MHz * (LCDC_FPR+1) / 2^20
	LCDC_FPR = 230685 = 0x3851D
	********************/
	LCD_Send_Cmd(0xE6);  // 7. Configure the dot clock frequency
	LCD_Send_Dat(0x03);
	LCD_Send_Dat(0x85);
	LCD_Send_Dat(0x1D);
	//8. Configure the LCD panel  
	//a. Set the panel size to 480 x 800 and polarity of LSHIFT, LLINE and LFRAME to active low 
	LCD_Send_Cmd(0xB0);
	if (LSHIFT) LCD_Send_Dat(0x0C); /* 0x08 0x0C 0xAE(5') */else LCD_Send_Dat(0xAE); //18bit panel, disable dithering, LSHIFT: Data latch in rising edge, LLINE and LFRAME: active low 
	LCD_Send_Dat(0x20);  /* 0x00 0x80 0x20(5') */    // TFT type 
	LCD_Send_Dat(0x03);     // Horizontal Width:  480 - 1 = 0x031F 
	LCD_Send_Dat(0x1F);
	LCD_Send_Dat(0x01);     // Vertical Width :  800 -1 = 0x01DF
	LCD_Send_Dat(0xDF);
	LCD_Send_Dat(0x2D);  /* 0x00 0x2d */   // 000 = режим RGB
											 //b. Set the horizontal period 
	LCD_Send_Cmd(0xB4); // Horizontal Display Period  
	LCD_Send_Dat(0x03);    // HT: horizontal total period (display + non-display) – 1 = 520-1 =  519 =0x0207
	LCD_Send_Dat(0xA0);
	LCD_Send_Dat(0x00);    // HPS: Horizontal Sync Pulse Start Position = Horizontal Pulse Width + Horizontal Back Porch = 16 = 0x10 
	LCD_Send_Dat(0x2E);
	LCD_Send_Dat(0x30);     // HPW: Horizontal Sync Pulse Width - 1=8-1=7 
	LCD_Send_Dat(0x00);    // LPS: Horizontal Display Period Start Position = 0x0000 
	LCD_Send_Dat(0x0F);
	LCD_Send_Dat(0x00);    // LPSPP: Horizontal Sync Pulse Subpixel Start Position(for serial TFT interface).  Dummy value for TFT interface. 
							 //c. Set the vertical period 
	LCD_Send_Cmd(0xB6);    // Vertical Display Period  
	LCD_Send_Dat(0x02);     // VT: Vertical Total (display + non-display) Period – 1  =647=0x287 
	LCD_Send_Dat(0x0D);
	LCD_Send_Dat(0x00);     // VPS: Vertical Sync Pulse Start Position  =     Vertical Pulse Width + Vertical Back Porch = 2+2=4  
	LCD_Send_Dat(0x10);
	LCD_Send_Dat(0x10);     //VPW: Vertical Sync Pulse Width – 1 =1 
	LCD_Send_Dat(0x08);     //FPS: Vertical Display Period Start Position = 0 
	LCD_Send_Dat(0x00);
	//9. Set the back light control PWM clock frequency
	//PWM signal frequency = PLL clock / (256 * (PWMF[7:0] + 1)) / 256
	LCD_Send_Cmd(0xBE);    // PWM configuration 
	LCD_Send_Dat(0x08);     // set PWM signal frequency to 170Hz when PLL frequency is 100MHz 
	LCD_Send_Dat(0xFF);     // PWM duty cycle  (50%) 
	LCD_Send_Dat(0x01);     // 0x09 = enable DBC, 0x01 = disable DBC  //on
	LCD_Send_Cmd(0x36);     // set address_mode
	if (MIRROR_H) LCD_Send_Dat(0x02); else if (MIRROR_V) LCD_Send_Dat(0x03);
	//13. Setup the MCU interface for 16-bit data write (565 RGB)
	LCD_Send_Cmd(0xF0);     // mcu interface config 
	LCD_Send_Dat(0x03);     // 16 bit interface (565)
							//10. Turn on the display 						
	LCD_Send_Cmd(LCD_DISPLAY_ON);     // display on 
	SSD1963_Bright(bright);
}

inline void SSD1963_Bright(uint8_t bright)
{
	LCD_Send_Cmd(0xBE);  // PWM configuration 
	LCD_Send_Dat(0x08);     // set PWM signal frequency to 170Hz when PLL frequency is 100MHz 
	LCD_Send_Dat(bright);   // PWM duty cycle  
	LCD_Send_Dat(0x01);
}
	
	void SSD1963_Test(void)
{
	LCD_Rect_Fill(0, 0, 800, 480, MAGENTA);
	LCD_Rect_Fill(1, 1, 798, 478, BLACK);
		
	char array[255];
		
	LCD_String(2, 100, GREEN, RED, tiny_8x8, "Test");	
	LCD_String(280, 10, BLUE, BLACK, tiny_8x8, "TEST");
	
	sprintf(array, "0123456789");
	LCD_String(2, 2, RED, BLUE, segment_seven_32x50_num, /*"9876543210"*/ array);
	
	sprintf(array, "0123456789");
	LCD_String_Scale_1B(2, 53, GREEN, BLACK, old_8x16, array/*"1234567890: TEST"*/, 3);
	
	LCD_Char_Scale_1B(3, 220, RED, BLACK, tiny_8x8,'R',2);
	LCD_Char_Scale_1B(20, 220, GREEN, BLACK, tiny_8x8,'G',2);
	LCD_Char_Scale_1B(37, 220, BLUE, BLACK, tiny_8x8,'B',2);
	
	LCD_String_Scale_1B(3, 120, BLUE, BLACK, tiny_8x8, "SCALE x3", 3);	
	
	LCD_Rect(260, 60, 50, 30, 2, GREEN);
	LCD_Line(25, 50, 100, 150, 2, MAGENTA);
	LCD_Triangle(50, 50, 50, 100, 150, 150, 2, BLUE);
	LCD_Circle(100, 100, 50, 0, 1, YELLOW);
	LCD_Circle(150, 150, 50, 1, 1, YELLOW);
	LCD_Rect_Round(235, 100, 80, 50, 10, 2, WHITE);
	LCD_Rect_Round_Fill(235, 180, 80, 50, 10, WHITE);

	LCD_String(220, 60, BLUE, BLACK, default_8x12, "Font default_8x12");

	LCD_Line(LCD_WIDTH -10-50, 10+25, LCD_WIDTH -10-50+50, 10+25, 1, WHITE);
	LCD_Line(LCD_WIDTH -10-25, 10, 		LCD_WIDTH -10-25, 	 10+50, 1, WHITE);	
}
