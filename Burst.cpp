#include "Burst.h"
#include <math.h>

void Burst::setupMode(uint8_t mode) {
	left = true;
	right = true;
	mirrored = false;
	fade = true;
	oneWay = false;

	switch (mode) {
		case 0: // Normal
		break;

		case 1: // Mirrored
		mirrored = true;
		break;

		case 2:
		oneWay = true;
		left = false;
		right = false;
		break;

		case 3:
		oneWay = true;
		left = true;
		right = false;
		break;

		case 4:
		oneWay = true;
		left = false;
		right = true;
		break;
		
	}

	fadeRate = 0.84 + ((float)random(120) / 1000);

	int s = random(0x100);
	bool allowSameColor = false;
	CRGB col;
	for (colorIndex = s; colorIndex < s + 0x100; colorIndex++) {
		col = Palettes.getColor(colorIndex % 0x100);
		if (col.r + col.g + col.b > 192) {
			allowSameColor = true;
			break;
		}
	}

	if ((random(6) == 0) && allowSameColor) {
		changeRate = 0;
		changeJump = 0;
	} else {
		changeRate = random(1, 9);
		changeJump = random(20, 37);
	}

	minBurstLength = random(10, 22);
	for (int i = 0; i < MAX_BURSTS; i++) {
		bursts[i].step = 0;
		bursts[i].totalSteps = 0;
		bursts[i].direction = 0x3;
	}
}

void Burst::addBurst() {
	for (int i = 0; i < MAX_BURSTS; i++) {
		if (bursts[i].totalSteps <= bursts[i].step) {
			bursts[i].colorIndex = (colorIndex += changeJump);
			bursts[i].index = random(pixelBuffer->getLength());
			bursts[i].step = 0;
			bursts[i].totalSteps = random(minBurstLength, minBurstLength + 5);

			if (oneWay) {
				if (!left && !right) {
					bursts[i].direction = random(1, 3);
				} else {
					bursts[i].direction = left | (right << 1);
				}
			} else {
				bursts[i].direction = 0x3;
			}

			break;
		}
	}
}

// Should be defined in math.h
#define M_PI_2 1.57079632679489661923 /* pi/2 */

void Burst::update(uint32_t ms) {
	if (fade) pixelBuffer->fade(fadeRate);

	uint16_t lenOffset = (pixelBuffer->getLength() / 20) >> (mirrored ? 1 : 0);

	if (random(max(minBurstLength - lenOffset, 1)) == 0) {
		this->addBurst();
	}

	for (int i = 0; i < MAX_BURSTS; i++) {
		if (bursts[i].totalSteps <= bursts[i].step)
			continue;

		//float lastStep = bursts[i].step - (bursts[i].stepRate / stepChangeRate);
		//uint8_t step = (uint8_t)bursts[i].step;
		float ratio = (float)bursts[i].step / (float)bursts[i].totalSteps;
		float offset = sin(ratio * M_PI_2) * (bursts[i].totalSteps >> 1);
		uint8_t step = (uint8_t)offset;
		
		if (true) {
			uint16_t a = (bursts[i].index + step) % pixelBuffer->getLength();
			uint16_t b = (pixelBuffer->getLength() + bursts[i].index - step) % pixelBuffer->getLength();

			CRGB col = Palettes.getColor(bursts[i].colorIndex);
			uint8_t scale = 255 * (1.0 - (ratio * ratio));
			col = col.nscale8(scale);

			if ((bursts[i].direction | 0x1) == bursts[i].direction) {
				if (mirrored) {
					pixelBuffer->setMirroredPixel(a, col);
				} else {
					pixelBuffer->setPixel(a, col);
				}
			}
			if ((bursts[i].direction | 0x2) == bursts[i].direction) {
				if (mirrored) {
					pixelBuffer->setMirroredPixel(b, col);
				} else {
					pixelBuffer->setPixel(b, col);
				}
			}
		}

		bursts[i].step++;
		bursts[i].colorIndex += changeRate;
		bursts[i].colorIndex %= 0x100;
	}

	colorIndex += changeRate;
	LightProgram::update(ms);
}
