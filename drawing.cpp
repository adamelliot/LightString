#include "drawing.h"

inline void setPixel8(int16_t x, int16_t y, CRGB col) {
	int8_t ax = x >> 8, ay = y >> 8;
	uint8_t fx0 = x & 0xff, fy0 = y & 0xff, fx1 = 0xff - fx0;
}

void vertLine(CRGB *pixels, int16_t x, int16_t y, int16_t len, CRGB col) {
	if (len == 0) return;
	
	if (len < 0) {
		y += (len + 1);
		len *= -1;
	}
	
	for (int i = 0; i < len; i++)
		pixels[xy(x, y + i)] = col;
}

void horzLine(CRGB *pixels, int16_t x, int16_t y, int16_t len, CRGB col) {
	if (len == 0) return;

	if (len < 0) {
		x += (len + 1);
		len *= -1;
	}

	for (int i = 0; i < len; i++)
		pixels[xy(x + i, y)] = col;
}

void drawRect(CRGB *pixels, int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB col) {
//	if (x0 > x1) SWAP(x0, x1);
	if (y0 > y1) SWAP(y0, y1);

	for (; y0 < y1; y0++) {
		horzLine(pixels, x0, y0, x1 - x0, col);
	}
}

// Ripped from arduino-tvout 
void lineTo(CRGB *pixels, int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB col) {
	//	if (x0 >= kMatrixWidth || y0 >= kMatrixHeight || x1 >= kMatrixWidth || y1 >= kMatrixHeight)
	//	return;

	if (x0 == x1) {
		y1 += y1 < y0 ? -1 : 1;
		vertLine(pixels, x0, y0, y1 - y0, col);
	} else if (y0 == y1) {
		x1 += x1 < x0 ? -1 : 1;
		horzLine(pixels, x0, y0, x1 - x0, col);
	} else {
		int e;
		signed int dx, dy,j, temp;
		signed char s1, s2, xchange;
		signed int x, y;

		x = x0;
		y = y0;

		//take absolute value
		if (x1 < x0) {
			dx = x0 - x1;
			s1 = -1;
		}
		else if (x1 == x0) {
			dx = 0;
			s1 = 0;
		}
		else {
			dx = x1 - x0;
			s1 = 1;
		}

		if (y1 < y0) {
			dy = y0 - y1;
			s2 = -1;
		}
		else if (y1 == y0) {
			dy = 0;
			s2 = 0;
		}
		else {
			dy = y1 - y0;
			s2 = 1;
		}

		xchange = 0;	 

		if (dy > dx) {
			temp = dx;
			dx = dy;
			dy = temp;
			xchange = 1;
		} 

		e = ((int)dy << 1) - dx;	

		for (j = 0; j <= dx; j++) {
			pixels[xy(x,y)] = col;

			if (e >= 0) {
				if (xchange == 1) x = x + s1;
				else y = y + s2;
				e = e - ((int)dx << 1);
			}
			if (xchange == 1)
				y = y + s2;
			else
				x = x + s1;
			e = e + ((int)dy << 1);
		}
	}
}

/* --- Old Code that's still useful --- */

// ============== Drawing Functions ==============
/*

// NOTE: Work in progress
void pixelBuffer::drawText(uint8_t x, uint8_t y, uint8_t textX, uint8_t textY, uint8_t width, uint8_t height, const char *str) {
	const int letterHeight = 15;

	uint32_t tmp;
	uint16_t offset, letterWidth, totalWidth = 0;
	uint8_t buffer[letterHeight];
	int letterIndex = 0;

	// TODO: Issue with letterIndex double moving after break
	for (letterIndex = 0; str[letterIndex] && textX; letterIndex++) {
		letterWidth = pgm_read_word(letters15Offsets + ((str[letterIndex] - ' ') << 1) + 1) + 1;

		if (textX > letterWidth)
			textX -= letterWidth;
		else break;
	}

	for (; str[letterIndex] && (totalWidth < width); letterIndex++) {
		//Serial.println(str[letterIndex] - ' ');

		tmp = pgm_read_dword(letters15Offsets + ((str[letterIndex] - ' ') << 1));

		letterWidth = (uint16_t)(tmp >> 16);
		offset = (uint16_t)(tmp & 0xffff);

//		Serial.println(offset);
//		Serial.println(letterWidth);
//		Serial.println("--");

		if (textX == (letterWidth - 1)) {
			drawColumn(x + totalWidth, y, letterHeight + 2, 0xff);
			totalWidth++;
		} else {
			for (; (textX < letterWidth) && (totalWidth < width); textX++) {
				memcpy_P(buffer, letters15 + offset + (letterHeight * textX), letterHeight);
			
				setPixelWithColorIndexAt(x + totalWidth, 0, 0xff);
				drawColumn(x + totalWidth, y + 1, letterHeight, buffer);
				setPixelWithColorIndexAt(x + totalWidth, 14, 0xff);
	//			drawColumn(x + totalWidth + 1, y, letterHeight, buffer);
				totalWidth++;
			}
		}
		textX = 0;
	}
	
//	Serial.println("= = =\n");
}

void pixelBuffer::drawColumn(uint8_t x, uint8_t y, uint8_t height, uint8_t *buffer) {
	uint16_t index = getIndex(x, y);
	int8_t factor = ((x % 2) == 1 ? 1 : -1);

	if (!height) return;
	if (y + height > this->height)
		height = this->height - y;

	for (int i = 0; i <= height; i++)
		((uint8_t *)pixels)[index + i * factor] = buffer[i];
}

void pixelBuffer::drawColumn(uint8_t x, uint8_t y, uint8_t height, uint8_t colIndex) {
	uint16_t index = getIndex(x, y);
	int8_t factor = ((x % 2) == 1 ? 1 : -1);

	if (!height) return;
	if (y + height > this->height)
		height = this->height - y;

	for (int i = 0; i < height; i++)
		((uint8_t *)pixels)[index + i * factor] = colIndex;
}

void pixelBuffer::drawColumn(uint8_t x, uint8_t y0, uint8_t y1, CRGB col) {
	uint16_t index = getIndex(x, y0);
	int8_t factor = ((x % 2) == 1 ? 1 : -1);

	if (y0 == y1) return;
	if (y0 > y1) {
		uint8_t swap = y0;
		y1 = y0;
		y0 = swap;
	}

	for (int i = 0; i <= y1 - y0; i++)
		pixels[index + i * factor] = col;
}

void pixelBuffer::drawRow(uint8_t x0, uint8_t x1, uint8_t y, CRGB col) {
	uint16_t index = getIndex(x0, y);
	int8_t swap, step, nextStep;

	if (x0 == x1) return;
	if (x0 > x1) {
		uint8_t swap = x0;
		x1 = x0;
		x0 = swap;
	}

	if ((x0 % 2) == 1) {
		step = ((height - 1) - y) * 2;
		nextStep = y * 2;
	} else {
		step = y * 2;
		nextStep = ((height - 1) - y) * 2;
	}

	for (int i = 0; i <= x1 - x0; i++) {
		pixels[index + i] = col;

		index += step;
		swap = step;
		step = nextStep;
		nextStep = swap;
	}
}

*/


