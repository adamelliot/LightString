#include "LSLPBlit.h"
#include "LSFont14.h"

LSLightProgram *factoryBlit(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPBlit(pixelBuffer, colorPalette, colorFunc);
}

LSLPBlit::LSLPBlit(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	pixelBuffer->setSize(15, 14);

	colorIndex = 0;
	indexStep = random(3) + 6;
}

void LSLPBlit::setConfig(void *_config) {
	pblit_config_t config = (pblit_config_t)_config;
}

void *LSLPBlit::getConfig() {
	return &config;
}

uint8_t LSLPBlit::getFrameRate() {
	return 30;
}

void LSLPBlit::update() {
//	uint16_t width, height;
//	const uint8_t *bits = blit_a(width, height);
	uint8_t bits[14];
	
	for (int x = 0; x < 16; x++) {
		memcpy_P(bits, letter_A + (x * 14), 14);
		for (int y = 0; y < 14; y++) {
			uint8_t val = (0xff - bits[y]) >> 1;
			
			//color_t col = colorPalette->getColor(colorIndex);
			//float ratio = val / 255.0;
			//col.channels[0] *= ratio;
			//col.channels[1] *= ratio;
			//col.channels[2] *= ratio;

			//pixelBuffer->setPixel(x, y, colorPalette->getColor(colorIndex + val));
			(pixelBuffer->*pixelBuffer->setIndexedPixelAt)(x, y, colorIndex + val);
		}
		//colorIndex += 4;
	}

	colorIndex += indexStep;

	LSLightProgram::update();
}

/*
uint8_t ret[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0x40, 0x00, 0x00, 0x00, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xdc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xcd, 0x00, 0x00, 0x00, 0x00, 0x00, 0xca, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0x1f, 0xb1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xde, 0x1e, 0x00, 0xb1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xe2, 0x04, 0x00, 0x00, 0x5a, 0x00, 0x00, 0x04, 0xe1, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x72, 0x2f, 0x07, 0x19, 0x54, 0xc9, 0xff, 0xdd, 0x1e, 0x00, 0x00, 0x1b, 0x27, 0x27, 0xb8, 0xfe, 0x27, 0x27, 0x27, 0xde, 0xa6, 0x34, 0x16, 0x82, 0xff, 0xff, 0xe7, 0x68, 0x31, 0x0c, 0x1d, 0x44, 0xa4, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0x74, 0x00, 0x00, 0x16, 0xf6, 0x1d, 0x00, 0x00, 0x73, 0xff, 0xff, 0xff, 0xff, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0xe3, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xfe, 0x00, 0x00, 0x00, 0x8a, 0x00, 0x00, 0x00, 0x72, 0xff, 0xc9, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xfe, 
	0xff, 0xff, 0xf2, 0x0f, 0x00, 0x00, 0x79, 0xff, 0x83, 0x00, 0x00, 0x0e, 0xf1, 0xff, 0xff, 0xea, 0x00, 0x00, 0x00, 0xd0, 0xf1, 0xaa, 0x9d, 0xe7, 0xad, 0x17, 0x00, 0x00, 0x78, 0xad, 0xad, 0xe4, 0xfe, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0xd2, 0xfb, 0x27, 0x00, 0x00, 0x1f, 0x89, 0x59, 0x02, 0x00, 0x00, 0x9c, 
	0xff, 0xff, 0x8e, 0x00, 0x00, 0x02, 0xe0, 0xff, 0xe6, 0x04, 0x00, 0x00, 0x8e, 0xff, 0xff, 0xff, 0x29, 0x00, 0x00, 0x29, 0xa5, 0xfc, 0xff, 0xff, 0xff, 0x22, 0x00, 0x00, 0xb1, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x06, 0x9e, 0xc5, 0x77, 0xff, 0xd1, 0x00, 0x00, 0x00, 0xbd, 0xff, 0xff, 0x42, 0x00, 0x00, 0x4c, 
	0xff, 0xfc, 0x1f, 0x00, 0x00, 0x15, 0x74, 0x74, 0x74, 0x19, 0x00, 0x00, 0x1f, 0xfc, 0xff, 0xff, 0xc4, 0x1e, 0x00, 0x00, 0x00, 0x26, 0xdb, 0xff, 0xff, 0x22, 0x00, 0x00, 0xb1, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x74, 0xff, 0xff, 0xff, 0xff, 0xaf, 0x00, 0x00, 0x01, 0xf5, 0xff, 0xff, 0x7c, 0x00, 0x00, 0x29, 
	0xff, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa8, 0xff, 0xff, 0xff, 0xf1, 0x83, 0x12, 0x00, 0x00, 0x37, 0xff, 0xff, 0x23, 0x00, 0x00, 0xa9, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0xd5, 0xff, 0xff, 0xff, 0xff, 0xdc, 0x00, 0x00, 0x00, 0xc5, 0xff, 0xff, 0x4b, 0x00, 0x00, 0x57, 
	0xff, 0x35, 0x00, 0x00, 0x1b, 0x74, 0x74, 0x74, 0x74, 0x74, 0x1c, 0x00, 0x00, 0x36, 0xff, 0xfb, 0x7d, 0xeb, 0xff, 0xb3, 0x00, 0x00, 0x0c, 0xff, 0xff, 0x3b, 0x00, 0x00, 0x35, 0x96, 0x5e, 0x80, 0xfe, 0x00, 0x00, 0x00, 0xd7, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1c, 0x00, 0x00, 0x32, 0xab, 0x7a, 0x07, 0x00, 0x00, 0x97, 
	0xc2, 0x00, 0x00, 0x00, 0x88, 0xff, 0xff, 0xff, 0xff, 0xff, 0x89, 0x00, 0x00, 0x00, 0xc3, 0xf9, 0x00, 0x00, 0x1a, 0x05, 0x00, 0x00, 0x48, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0xfe, 0x00, 0x00, 0x00, 0xd7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc5, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0xfa, 
	0x4f, 0x00, 0x00, 0x07, 0xea, 0xff, 0xff, 0xff, 0xff, 0xff, 0xea, 0x06, 0x00, 0x00, 0x51, 0xfd, 0x60, 0x15, 0x00, 0x00, 0x01, 0x4c, 0xe5, 0xff, 0xff, 0xf6, 0x5f, 0x03, 0x00, 0x01, 0x29, 0xab, 0xfe, 0x00, 0x00, 0x00, 0xd7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xce, 0x53, 0x1c, 0x00, 0x07, 0x37, 0x7f, 0xfc, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xdf, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xe0, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe9, 0xf9, 0xff, 0xff, 0xff, 0xff, 
};
*/
