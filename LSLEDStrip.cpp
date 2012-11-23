#include "LSLEDStrip.h"
#include "SPI.h"

// Constructor for use with arbitrary clock/data pins:
LSLEDStrip::LSLEDStrip(uint16_t n, uint8_t dpin, uint8_t cpin, uint8_t flags) 
	: flags(flags)
{
	updatePins(dpin, cpin);
	alloc(n);
}

LSLEDStrip::LSLEDStrip(uint16_t n, uint8_t flags) 
	: flags(flags) 
{
	updatePins();
	alloc(n);
}

// Allocate 3 bytes per pixel, init to RGB 'off' state:
void LSLEDStrip::alloc(uint16_t n) {
	length = n;

	if ((flags | INDEXED_PIXEL_BUFFER) == flags) {
		pixels = calloc(n, 1);
		updateFunc = useHardwareSPI ? &LSLEDStrip::updateIndexedPixelsWithSPI : &LSLEDStrip::updateIndexedPixels;
	} else {
		pixels = calloc(n, 3);
		updateFunc = useHardwareSPI ? &LSLEDStrip::updatePixelsWithSPI : &LSLEDStrip::updatePixels;
	}
}

// Change pin assignments post-constructor, using arbitrary pins:
void LSLEDStrip::updatePins(uint8_t dpin, uint8_t cpin) {
	pinMode(dpin, OUTPUT);
	pinMode(cpin, OUTPUT);

  // Note: any prior clock/data pin directions are left as-is and are
  // NOT restored as inputs!

	useHardwareSPI = false;
	datapin        = dpin;
	clkpin         = cpin;
	clkport        = portOutputRegister(digitalPinToPort(cpin));
	clkpinmask     = digitalPinToBitMask(cpin);
	dataport       = portOutputRegister(digitalPinToPort(dpin));
	datapinmask    = digitalPinToBitMask(dpin);
}

void LSLEDStrip::updatePins(void) {
	useHardwareSPI = true;
	datapin = clkpin = 0;

	// Taken from WS2801 sample code
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV8);  // 2 MHz
	// WS2801 datasheet recommends max SPI clock of 2 MHz, and 50 Ohm
	// resistors on SPI lines for impedance matching.  In practice and
	// at short distances, 2 MHz seemed to work reliably enough without
	// resistors, and 4 MHz was possible with a 220 Ohm resistor on the
	// SPI clock line only.  Your mileage may vary.  Experiment!
	// SPI.setClockDivider(SPI_CLOCK_DIV4);  // 4 MHz
}

LSPixelBuffer *LSLEDStrip::getPixelBuffer(uint16_t length, uint16_t offset) {
	if (length == 0) length = this->length;

	if ((flags | INDEXED_PIXEL_BUFFER) != flags) {
		offset *= 3;
	}

	LSPixelBuffer *ret = new LSPixelBuffer((void *)((uint8_t *)pixels + offset), length, flags);
	ret->clear();

	return ret;
}

uint16_t LSLEDStrip::getLength(void) {
  return length;
}

void *LSLEDStrip::getPixels(void) {
	return pixels;
}

LSColorPalette *LSLEDStrip::getColorPalette(void) {
	return colorPalette;
}

void LSLEDStrip::setColorPalette(LSColorPalette *colorPalette) {
	this->colorPalette = colorPalette;
}

bool LSLEDStrip::useIndexedPixelBuffer() {
	return (flags | INDEXED_PIXEL_BUFFER) == flags;
}

void LSLEDStrip::updatePixels() {
	uint16_t i, nl3 = length * 3;
	uint8_t bit;
	uint8_t *pixels = (uint8_t *)this->pixels;

	for (i = 0; i < nl3; i++) {
		for (bit = 0x80; bit; bit >>= 1) {
			if (pixels[i] & bit) *this->dataport |=  this->datapinmask;
			else                 *this->dataport &= ~this->datapinmask;
			*this->clkport |=  this->clkpinmask;
			*this->clkport &= ~this->clkpinmask;
		}
	}

	delay(1);
}

void LSLEDStrip::updateIndexedPixels() {
	uint16_t i, j, len = length;
	uint8_t bit;
	uint8_t *pixels = (uint8_t *)this->pixels;
	pcolor_t paletteTable = colorPalette->getPaletteTable();
	color_t col;

	for (i = 0; i < len; i++) {
		col = paletteTable[pixels[i]];
		for (j = 0; j < 2; j++) {
			for (bit = 0x80; bit; bit >>= 1) {
				if (col.channels[j] & bit) *this->dataport |=  this->datapinmask;
				else                       *this->dataport &= ~this->datapinmask;
				*this->clkport |=  this->clkpinmask;
				*this->clkport &= ~this->clkpinmask;
			}
		}
	}

	delay(1);
}

void LSLEDStrip::updatePixelsWithSPI() {
	uint16_t i, nl3 = length * 3;
	uint8_t *pixels = (uint8_t *)this->pixels;

	for (i=0; i<nl3; i++) {
		SPDR = pixels[i];
		while(!(SPSR & (1<<SPIF)));
	}

	delay(1);
}

void LSLEDStrip::updateIndexedPixelsWithSPI() {
	uint16_t i, j, len = length;
	uint8_t *pixels = (uint8_t *)this->pixels;
	pcolor_t paletteTable = colorPalette->getPaletteTable();
	color_t col;

	for (i = 0; i < len; i++) {
		col = paletteTable[pixels[i]];
		for (j = 0; j < 3; j++) {
			SPDR = col.channels[j];
			while(!(SPSR & (1<<SPIF)));
		}
	}

	delay(1);
}
