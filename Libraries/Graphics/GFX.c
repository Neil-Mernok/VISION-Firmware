/***********************************
 This is a graphics core library, for use with monochrome displays. 

 BSD license
 Kobus Goosen, June 2014
 ****************************************/

#include "GFX.h"
#include "glcdfont.c"
//#include "Includes.h"

int16_t WIDTH = 128, HEIGHT = 64;     // this is the 'raw' display w/h - never changes
int16_t _width = 128, _height = 64;       // dependent on rotation
int16_t cursor_x = 0, cursor_y = 0;
uint16_t textcolor = 0xFFFF, textbgcolor = 0xFFFF;
uint8_t textsize = 1;
uint8_t rotation = 0;
boolean wrap = 1;           // If set, 'wrap' text at right edge of display

///////////////////////////
FONT_INFO Font;
uint16_t start_of_line = 0;
///////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// Kobus's functions
void writeF(uint8_t c);

int print_text(char* text)
{
	int i=0;

	while(*text != 0)
	{
		writeF(*text);
		text++;
		i++;
		if(i>500) break;
	}
	return i;
}

void print_TextXY(char* text)
{
//    int x_pos = 0;
//
//    x_pos = (get_width()-get_string_width(text))/2;
//    if(x_pos > 0)
//        cursor_x = x_pos;
//    else
//        cursor_x = 0;

    //puts(text);
    print_text(text);
}

void print_centered(char* text)
{
    int x_pos = 0;
    
    x_pos = (get_width()-get_string_width(text))/2;
    if(x_pos > 0)
        cursor_x = x_pos;
    else
        cursor_x = 0;
    
    //puts(text);
    print_text(text);
}


void Setfont(const FONT_INFO *F)
{
	Font = *F;
}

void drawImage(const IMAGE_INFO *I, int16_t x, int16_t y, uint16_t color)
{
	drawBitmap(x, y, I->bitmapB, I->W, I->H, color);
}

void drawCharf(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg)
{
	uint8_t* buffer = NULL;
	int16_t i, j, byteWidth;

	int ss, xx, yy, zz, ofs;
	FONT_CHAR_INFO ch, ch1;
	uint8_t datas[50];

	ofs = (c - Font.start_char);
	xx = Font.start_char;
	yy = Font.characters[ofs].offset;
	zz = Font.characters[ofs].width;
	ch = Font.characters[ofs];
	ch1 = Font.characters[2];
	ss = Font.font_height;

	if (/*(Font == NULL) || */(c > Font.end_char) || (c < Font.start_char))
		return;
	else
	{
		buffer = Font.bitmaps + Font.characters[c - Font.start_char].offset;

		memcpy(datas, buffer, 8);

		//drawBitmap(x, y, buffer, Font.characters[ofs].width, Font.font_height, color);

		byteWidth = (Font.characters[ofs].width + 7) / 8;

		if (color == bg)
		{
			for (j = 0; j < Font.font_height; j++)
			{
				for (i = 0; i < Font.characters[ofs].width; i++)
				{
					if (buffer[j * byteWidth + (i >> 3)] & (0x80 >> (i & 7)))
					{
						drawPixel(x + i, y + j, color);
					}
					//else
					//{
					//	drawPixel(x + i, y + j, bg);
					//}
				}
			}
		}
		else
		{
			for (j = 0; j < Font.font_height; j++)
			{
				for (i = 0; i < Font.characters[ofs].width; i++)
				{
					if (buffer[j * byteWidth + (i >> 3)] & (0x80 >> (i & 7)))
					{
						drawPixel(x + i, y + j, color);
					}
					else
					{
						drawPixel(x + i, y + j, bg);
					}
				}
			}
		}
	}
}

void writeF(uint8_t c)
{
	if (c == '\n')
	{
		cursor_y += Font.font_height;
		cursor_x = start_of_line;
	}
	else if (c == '\r')
	{
		// skip em
	}
	else
	{
		if (wrap && (cursor_x > (_width - Font.characters[c - (Font.start_char)].width)))
		{
			cursor_y += Font.font_height;
			cursor_x = start_of_line;
		}
		drawCharf(cursor_x, cursor_y, c, textcolor, textbgcolor);
		cursor_x += Font.characters[c - (Font.start_char)].width;
	}
}

uint8_t get_char_width(char c)
{
	return Font.characters[c - (Font.start_char)].width;
}

int get_string_width(char* s)
{
	int width = 0;
	do
	{
		width += get_char_width(*s++);
	} while ((*s != 0)&&(*s != '\n'));
	return width;
}

uint8_t get_font_h(void)
{
    return Font.font_height;
}
///////////////////////////////////////////////////////////////////////////////////





// draw a circle outline
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	drawPixel(x0, y0 + r, color);
	drawPixel(x0, y0 - r, color);
	drawPixel(x0 + r, y0, color);
	drawPixel(x0 - r, y0, color);

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 + x, y0 - y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + y, y0 + x, color);
		drawPixel(x0 - y, y0 + x, color);
		drawPixel(x0 + y, y0 - x, color);
		drawPixel(x0 - y, y0 - x, color);

	}
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4)
		{
			drawPixel(x0 + x, y0 + y, color);
			drawPixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2)
		{
			drawPixel(x0 + x, y0 - y, color);
			drawPixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8)
		{
			drawPixel(x0 - y, y0 + x, color);
			drawPixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1)
		{
			drawPixel(x0 - y, y0 - x, color);
			drawPixel(x0 - x, y0 - y, color);
		}
	}
}

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	drawFastVLine(x0, y0 - r, 2 * r + 1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

// used to do circles and roundrects!
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1)
		{
			drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}
		if (cornername & 0x2)
		{
			drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}

// bresenham's algorithm - thx wikpedia
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	int16_t dx, dy;
	int16_t err;
	int16_t ystep;

	int16_t steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep)
	{
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	dx = x1 - x0;
	dy = abs(y1 - y0);

	err = dx / 2;

	if (y0 < y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for (; x0 <= x1; x0++)
	{
		if (steep)
		{
			drawPixel(y0, x0, color);
		}
		else
		{
			drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}

// draw a rectangle
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	drawFastHLine(x, y, w, color);
	drawFastHLine(x, y + h - 1, w, color);
	drawFastVLine(x, y, h, color);
	drawFastVLine(x + w - 1, y, h, color);
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	// stupidest version - update in subclasses if desired!
	drawLine(x, y, x, y + h - 1, color);
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	// stupidest version - update in subclasses if desired!
	drawLine(x, y, x + w - 1, y, color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	int16_t i;
	// stupidest version - update in subclasses if desired!
	for (i = x; i < x + w; i++)
	{
		drawFastVLine(i, y, h, color);
	}
}

void fillScreen(uint16_t color)
{
	fillRect(0, 0, _width, _height, color);
}

// draw a rounded rectangle!
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	// smarter version
	drawFastHLine(x + r, y, w - 2 * r, color); // Top
	drawFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
	drawFastVLine(x, y + r, h - 2 * r, color); // Left
	drawFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
	// draw four corners
	drawCircleHelper(x + r, y + r, r, 1, color);
	drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
	drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

// fill a rounded rectangle!
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
	// smarter version
	fillRect(x + r, y, w - 2 * r, h, color);

	// draw four corners
	fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

// draw a triangle!
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	drawLine(x0, y0, x1, y1, color);
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x0, y0, color);
}

// fill a triangle!
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{

	int16_t a, b, y, last;
	int16_t dx01, dy01, dx02, dy02, dx12, dy12, sa, sb;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1)
	{
		swap(y0, y1);
		swap(x0, x1);
	}
	if (y1 > y2)
	{
		swap(y2, y1);
		swap(x2, x1);
	}
	if (y0 > y1)
	{
		swap(y0, y1);
		swap(x0, x1);
	}

	if (y0 == y2)
	{ // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)
			a = x1;
		else if (x1 > b)
			b = x1;
		if (x2 < a)
			a = x2;
		else if (x2 > b)
			b = x2;
		drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;
	sa = 0;
	sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2)
		last = y1;   // Include y1 scanline
	else
		last = y1 - 1; // Skip it

	for (y = y0; y <= last; y++)
	{
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		 a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		 b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		 */
		if (a > b)
			swap(a, b);
		drawFastHLine(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++)
	{
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		 a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		 b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		 */
		if (a > b)
			swap(a, b);
		drawFastHLine(a, y, b - a + 1, color);
	}
}

void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
	int16_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (bitmap[j * byteWidth + (i >> 3)] & (128 >> (i & 7)))
			{
				drawPixel(x + i, y + j, color);
			}
		}
	}
}

void write(uint8_t c)
{
	if (c == '\n')
	{
		cursor_y += textsize * 8;
		cursor_x = 0;
	}
	else if (c == '\r')
	{
		// skip em
	}
	else
	{
		drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
		cursor_x += textsize * 6;
		if (wrap && (cursor_x > (_width - textsize * 6)))
		{
			cursor_y += textsize * 8;
			cursor_x = 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//// this function received references to SDIO like printf() and puts()
//char putchar(char c)
//{
//	writeF(c);
//	//write(c);
//	return c;
//}
///////////////////////////////////////////////////////////////////////////////////////////////////

// draw a character
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
	int8_t i, j;

	if ((x >= _width) || // Clip right
			(y >= _height) || // Clip bottom
			((x + 5 * size - 1) < 0) || // Clip left
			((y + 8 * size - 1) < 0))   // Clip top
		return;

	for (i = 0; i < 6; i++)
	{
		uint8_t line;
		if (i == 5)
			line = 0x0;
		else
			line = font[(c * 5) + i];
		for (j = 0; j < 8; j++)
		{
			if (line & 0x1)
			{
				if (size == 1) // default size
					drawPixel(x + i, y + j, color);
				else
				{  // big size
					fillRect(x + (i * size), y + (j * size), size, size, color);
				}
			}
			else if (bg != color)
			{
				if (size == 1) // default size
					drawPixel(x + i, y + j, bg);
				else
				{  // big size
					fillRect(x + i * size, y + j * size, size, size, bg);
				}
			}
			line >>= 1;
		}
	}
}

void setCursor(int16_t x, int16_t y)
{
	start_of_line = x;
	cursor_x = x;
	cursor_y = y;
}

void setTextSize(uint8_t s)
{
	textsize = (s > 0) ? s : 1;
}

void setTextColor(uint16_t c)
{
	textcolor = c;
	textbgcolor = c;
	// for 'transparent' background, we'll set the bg 
	// to the same as fg instead of using a flag
}

void setTextColorBG(uint16_t c, uint16_t bg)
{
	textcolor = c;
	textbgcolor = bg;
}

void setTextWrap(boolean w)
{
	wrap = w;
}

uint8_t getRotation(void)
{
	rotation %= 4;
	return rotation;
}

void setRotation(uint8_t x)
{
	x %= 4;  // cant be higher than 3
	rotation = x;
	switch (x)
	{
	case 0:
	case 2:
		_width = WIDTH;
		_height = HEIGHT;
		break;
	case 1:
	case 3:
		_width = HEIGHT;
		_height = WIDTH;
		break;
	}
}

//void  invertDisplay(boolean i) {
//  // do nothing, can be subclassed
//}

// return the size of the display which depends on the rotation!
int16_t get_width(void)
{
	return _width;
}

int16_t get_height(void)
{
	return _height;
}
