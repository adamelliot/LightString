#include "LSCPGradientPalette.h"
#include <avr/pgmspace.h>

LSColorPalette *factoryGradientPalette() {
	return new LSCPGradientPalette();
}

LSCPGradientPalette::LSCPGradientPalette() 
	: LSColorPalette()
{
	colors[0] = randomColor();
	colors[1] = randomColor();
	colors[2] = randomColor();
	
	sections = 2;
	mirrored = false;	
}

void LSCPGradientPalette::setConfig(void *_config) {
	pgradient_palette_func func = (pgradient_palette_func)_config;
	gradient_palette_t config = func();

	memcpy(this->colors, config.colors, sizeof(color_t) * MAX_GRADIENT_COLORS);
	
	this->sections = config.sections;
	this->mirrored = config.mirrored;
}

color_t LSCPGradientPalette::interpolate(color_t col1, color_t col2, float weight) {
	color_t ret;
	
	ret.channels[0] = (col1.channels[0] * (1.0 - weight)) + (col2.channels[0] * weight);
	ret.channels[1] = (col1.channels[1] * (1.0 - weight)) + (col2.channels[1] * weight);
	ret.channels[2] = (col1.channels[2] * (1.0 - weight)) + (col2.channels[2] * weight);
	
	return ret;
}

color_t LSCPGradientPalette::getColor(uint8_t index) {
	uint16_t totalColors = 0x100;
	
	if (mirrored == true) {
		if (index > (totalColors >> 1))
			return getColor(totalColors - index);
		totalColors >>= 1;
	}

	uint16_t indexSections = (index * sections);
	uint8_t section = indexSections / totalColors;
	
	float weight = ((float)indexSections / (float)totalColors) - section;
	color_t col = interpolate(colors[section], colors[section + 1], weight);

	return colorFunc(col.channels[0], col.channels[1], col.channels[2]);
}

// === Palettes ===

gradient_palette_t rainbowGradient() {
	return (gradient_palette_t){
		{(color_t){255, 0, 0}, (color_t){0, 255, 0}, (color_t){0, 0, 255}, (color_t){255, 0, 0}},
		3,
		false
	};
}

gradient_palette_t rainbowBlackGradient() {
	return (gradient_palette_t){
		{(color_t){255, 0, 0}, (color_t){0, 255, 0}, (color_t){0, 0, 255}, (color_t){0, 0, 0}, (color_t){255, 0, 0}},
	4,
	false
	};
}
/*
gradient_palette_t rainbowBlackBlackGradient() {
	return (gradient_palette_t){
		{(color_t){255, 0, 0}, (color_t){0, 0, 0}, (color_t){0, 0, 0}, (color_t){0, 255, 0}, (color_t){0, 0, 0}, (color_t){0, 0, 0}, (color_t){0, 0, 255}, (color_t){0, 0, 0}, (color_t){0, 0, 0}, (color_t){255, 0, 0}},
		9,
		false
	};
}
*/
gradient_palette_t pinkPurpleGradient() {
	return (gradient_palette_t){
		{(color_t){255, 0, 128}, (color_t){96, 0, 192}},
		1,
		true
	};
}

gradient_palette_t solidRedGradient() {
	return (gradient_palette_t){
		{(color_t){255, 192, 0}, (color_t){255, 192, 0}},
		1,
		true
	};
}

gradient_palette_t pinkPurpleBlackGradient() {
	return (gradient_palette_t){
		{(color_t){255, 0, 128}, (color_t){96, 0, 192}, (color_t){0, 0, 0}, (color_t){0, 0, 0}, (color_t){255, 0, 128}},
		4,
		false
	};
}

gradient_palette_t cyanPinkGradient() {
	return (gradient_palette_t){
		{(color_t){0, 255, 255}, (color_t){255, 0, 96}},
		1,
		true
	};
}

gradient_palette_t cyanPinkBlackGradient() {
	return (gradient_palette_t){
		{(color_t){0, 255, 192}, (color_t){0, 0, 0}, (color_t){255, 16, 64}, (color_t){0, 0, 0}, (color_t){0, 255, 192}},
		4,
		false
	};
}

gradient_palette_t blueGreenGradient() {
	return (gradient_palette_t){
		{(color_t){0, 0, 255}, (color_t){0, 255, 64}},
		1,
		true
	};
}

gradient_palette_t yellowGradient() {
	return (gradient_palette_t){
		{(color_t){255, 255, 0}, (color_t){64, 64, 0}},
		1,
		true
	};
}

gradient_palette_t whiteGradient() {
	return (gradient_palette_t){
		{(color_t){255, 255, 255}, (color_t){0, 0, 0}},
		1,
		true
	};
}

gradient_palette_t greenBlueBlackGradient() {
	return (gradient_palette_t){
		{(color_t){0, 255, 64}, (color_t){0, 0, 255}, (color_t){0, 0, 0}, (color_t){0, 0, 0}, (color_t){0, 255, 64}},
		4,
		false
	};
}

gradient_palette_t greenBlueGradient() {
	return (gradient_palette_t){
		{(color_t){0, 255, 64}, (color_t){0, 0, 255}, (color_t){0, 255, 64}},
		2,
		false
	};
}

gradient_palette_t redOrgangeGradient() {
	return (gradient_palette_t){
		{(color_t){128, 128, 128}, (color_t){128, 31, 0}, (color_t){128, 95, 0}},
		2,
		true
	};
}

gradient_palette_t redOrgangeBlackGradient() {
	return (gradient_palette_t){
		{(color_t){255, 31, 0}, (color_t){255, 191, 0}, (color_t){0, 0, 0}, (color_t){0, 0, 0}, (color_t){255, 31, 0}},
		4,
		false
	};
}

gradient_palette_t redWhiteBlackGradient() {
	return (gradient_palette_t){
		{(color_t){255, 31, 0}, (color_t){255, 0, 0}, (color_t){255, 255, 255}, (color_t){0, 0, 0}, /*(color_t){0, 0, 0}, */(color_t){255, 31, 0}},
		6,
		false
	};
}

gradient_palette_t redWhiteGradient() {
	return (gradient_palette_t){
		{(color_t){255, 31, 0}, (color_t){255, 0, 0}, (color_t){255, 255, 255}, (color_t){255, 31, 0}},
		3,
		false
	};
}

gradient_palette_t blueWhiteBlackGradient() {
	return (gradient_palette_t){
		{(color_t){0, 31, 255}, (color_t){255, 255, 255}, (color_t){0, 0, 0}, (color_t){0, 0, 0}, (color_t){0, 31, 255}},
		4,
		false
	};
}

gradient_palette_t blueWhiteGradient() {
	return (gradient_palette_t){
		{(color_t){0, 31, 255}, (color_t){255, 255, 255}, (color_t){0, 31, 255}},
		2,
		false
	};
}

gradient_palette_t blueYellowBlackGradient() {
	return (gradient_palette_t){
		{(color_t){0, 95, 255}, (color_t){255, 255, 0}, (color_t){0, 0, 0}, (color_t){0, 0, 0}, (color_t){0, 31, 255}},
		4,
		false
	};
}

gradient_palette_t blueYellowBlackHardGradient() {
	return (gradient_palette_t){
		{(color_t){0, 0, 0}, (color_t){16, 16, 0}, (color_t){255, 255, 0}, (color_t){0, 95, 255}},
		3,
		false
	};
}

gradient_palette_t blueYellowGradient() {
	return (gradient_palette_t){
		{(color_t){0, 95, 255}, (color_t){255, 255, 0}, (color_t){0, 31, 255}},
		2,
		false
	};
}

gradient_palette_t whiteSolidGradient() {
	return (gradient_palette_t){
		{(color_t){64, 64, 64}, (color_t){64, 64, 64}},
		1,
		false
	};
}
