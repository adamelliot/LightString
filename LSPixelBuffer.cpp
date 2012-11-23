#include "LSPixelBuffer.h"

LSPixelBuffer::LSPixelBuffer(void *pixels, uint16_t length, uint8_t flags)
	: pixels(pixels), length(length), flags(flags)
{
	pixelBytes = ((flags | INDEXED_PIXEL_BUFFER) == flags) ? 1 : 3;
	bytes = length * pixelBytes;
	
	if ((flags | INDEXED_PIXEL_BUFFER) == flags) {
		setIndexedPixel = setPixelWithColorIndex;
		setMirroredIndexedPixel = setMirroredPixelWithColorIndex;
		setIndexedPixelAt = setPixelWithColorIndexAt;
	} else {
		setIndexedPixel = setPixelWithPaletteIndex;
		setMirroredIndexedPixel = setMirroredPixelWithPaletteIndex;
		setIndexedPixelAt = setPixelWithPaletteIndexAt;
	}
}

bool LSPixelBuffer::useIndexedPixelBuffer() {
	return (flags | INDEXED_PIXEL_BUFFER) == flags;
}

void *LSPixelBuffer::getPixels() {
	return pixels;
}

uint16_t LSPixelBuffer::getLength() {
	return length;
}

void LSPixelBuffer::setSize(uint16_t width, uint16_t height) {
	this->width = width;
	this->height = height;
}

uint16_t LSPixelBuffer::getWidth() {
	return this->width;
}

uint16_t LSPixelBuffer::getHeight() {
	return this->height;
}

// TODO: Make this adjustable based on orientation
uint16_t LSPixelBuffer::getIndex(uint8_t x, uint8_t y) {
	return (height * x) + ((x % 2) == 1 ? y : (height - 1) - y);
}

// ============== Setting Colored Pixel Functions ==============

void LSPixelBuffer::setPixel(uint16_t index, color_t col) {
	((pcolor_t)pixels)[index] = col;
}

void LSPixelBuffer::setMirroredPixel(uint16_t index, color_t col) {
	((pcolor_t)pixels)[index] = col;
	((pcolor_t)pixels)[length - index - 1] = col;
}

void LSPixelBuffer::setPixelAt(uint8_t x, uint8_t y, color_t col) {
	((pcolor_t)pixels)[getIndex(x, y)] = col;
}

// ============== Setting Indexed Pixel Functions ==============

void LSPixelBuffer::setPixelWithColorIndex(uint16_t index, uint8_t colIndex) {
	((uint8_t *)pixels)[index] = colIndex;
}

void LSPixelBuffer::setPixelWithPaletteIndex(uint16_t index, uint8_t colIndex) {
	((pcolor_t *)pixels)[index] = colorPalette->getColor(colIndex);
}

void LSPixelBuffer::setMirroredPixelWithColorIndex(uint16_t index, uint8_t colIndex) {
	((uint8_t *)pixels)[index] = colIndex;
	((uint8_t *)pixels)[length - index - 1] = colIndex;
}

void LSPixelBuffer::setMirroredPixelWithPaletteIndex(uint16_t index, uint8_t colIndex) {
	color_t col = colorPalette->getColor(colIndex);

	((pcolor_t *)pixels)[index] = col;
	((pcolor_t *)pixels)[length - index - 1] = col;
}

void LSPixelBuffer::setPixelWithColorIndexAt(uint8_t x, uint8_t y, uint8_t colIndex) {
	((uint8_t *)pixels)[getIndex(x, y)] = colIndex;
}

void LSPixelBuffer::setPixelWithPaletteIndexAt(uint8_t x, uint8_t y, uint8_t colIndex) {
	((pcolor_t *)pixels)[getIndex(x, y)] = colorPalette->getColor(colIndex);
}

// ============== Pixel Retrival Functions ==============

void *LSPixelBuffer::getPixel(uint16_t index) {
	return pixels + (index * pixelBytes);
}

// TODO: Fix this method
void *LSPixelBuffer::getPixelAt(uint8_t x, uint8_t y) {
	return 0;
}

// ============== Full Buffer Modifiers ==============

/**
 * Fast fade doesn't use FP, just shift, only allows 8 steps.
 */
void LSPixelBuffer::fade(uint8_t shift) {
	for (int i = 0; i < bytes; i++) {
		((uint8_t *)pixels)[i] >>= shift;
	}
}

void LSPixelBuffer::fade(float ratio) {
	for (int i = 0; i < bytes; i++) {
		((uint8_t *)pixels)[i] *= ratio;
	}
}

void LSPixelBuffer::clear() {
	for (int i = 0; i < bytes; i++)
		((uint8_t *)pixels)[i] = 0;
}

void LSPixelBuffer::clear(color_t col) {
	for (int i = 0; i < length; i++)
		((pcolor_t)pixels)[i] = col;
}

void LSPixelBuffer::shiftUp(uint16_t by) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height - 1; y++) {
			// TODO: Make this not use getIndex
			((pcolor_t)pixels)[getIndex(x, y)] = ((pcolor_t)pixels)[getIndex(x, y + 1)];
		}
	}
}

void LSPixelBuffer::shiftDown(uint16_t by) {
	
}

// ============== Line Drawing Functions ==============

void LSPixelBuffer::drawColumn(uint8_t x, uint8_t y0, uint8_t y1, color_t col) {
	uint16_t index = getIndex(x, y0);
	int8_t factor = ((x % 2) == 1 ? 1 : -1);

	if (y0 == y1) return;
	if (y0 > y1) {
		uint8_t swap = y0;
		y1 = y0;
		y0 = swap;
	}

	for (int i = 0; i <= y1 - y0; i++)
		((pcolor_t)pixels)[index + i * factor] = col;
}

void LSPixelBuffer::drawRow(uint8_t x0, uint8_t x1, uint8_t y, color_t col) {
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
		((pcolor_t)pixels)[index + i] = col;

		index += step;
		swap = step;
		step = nextStep;
		nextStep = swap;
	}
}

// Ripped from arduino-tvout 
void LSPixelBuffer::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, color_t col) {
	if (x0 >= width || y0 >= height || x1 >= width || y1 >= height)
		return;

	if (x0 == x1) {
		drawColumn(x0, y0, y1, col);
	} else if (y0 == y1)
		drawRow(x0, x1, y0, col);
	else {
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
			setPixel(x, y, col);

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
