#ifndef FONT_H_
#define FONT_H_
#include "stm32f4xx.h"

#define FONT      cousine_14ptcBitmaps  // прописать нужный шрифт
#define FONT_info cousine_14ptcFontInfo // описание шрифта, размер и т.д. см.в конце файла шрифта
#define indent 5   // точек (px)        // сколько точек остается поле после символа (отступ справа от символа до следующего символа)


//#define FONT      cousine_16ptcBitmaps  // прописать нужный шрифт
//#define FONT_info cousine_16ptcFontInfo // описание шрифта, размер и т.д. см.в конце файла шрифта
//#define indent 5   // точек (px)        // сколько точек остается поле после символа (отступ справа от символа до следующего символа)

extern const unsigned char cousine_14ptcBitmaps[];
extern const unsigned char cousine_14ptcFontInfo[];	

extern const unsigned char cousine_18ptcBitmaps[];
extern const unsigned char cousine_18ptcFontInfo[];	

extern const unsigned char cousine_16ptcBitmaps[];
extern const unsigned char cousine_16ptcFontInfo[];	



/*  
// wi ширина символа, бит */
// he количество строк  */
// skip пропуск количества симв., между 127-м и 192-м по ANSI */
#endif /* FONT_H_ */

