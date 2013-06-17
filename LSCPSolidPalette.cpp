#include "LSCPSolidPalette.h"

LSColorPalette *factorySolidPalette() {
	return new LSCPSolidPalette();
}

void LSCPSolidPalette::setConfig(void *config) {
	color = *((pcolor_t)config);
}

color_t LSCPSolidPalette::getColor(uint8_t index) {
	return colorFunc(color.channels[0], color.channels[1], color.channels[2]);
}
