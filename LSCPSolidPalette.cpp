#include "LSCPSolidPalette.h"

LSColorPalette *factorySolidPalette() {
	return new LSCPSolidPalette();
}

void LSCPSolidPalette::setConfig(void *config) {
	color = *((CRGB *)config);
}

CRGB LSCPSolidPalette::getColor(uint8_t index) {
	return color;
}
