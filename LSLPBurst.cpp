#include "LSLPBounce.h"

LSLightProgram *factoryBurst(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPBurst(pixelBuffer, colorPalette, colorFunc);
}

void LSLPBurst::setupMode(uint8_t mode) {
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

	stepChangeRate = 0.80 + ((float)random(110) / 1000);
	fadeRate = 0.70 + ((float)random(301) / 1000);
	
	colorIndex = 0; // random(0xff);
	changeRate = random(7) + 1;
	changeJump = random(16) + 20;

	minBurstLength = random(7) + 7;
	for (int i = 0; i < MAX_BURSTS; i++) {
		bursts[i].step = 0;
		bursts[i].stepsLeft = 0;
	}
}

void LSLPBurst::addBurst() {
	for (int i = 0; i < MAX_BURSTS; i++) {
		if (bursts[i].stepsLeft == 0) {
			bursts[i].colorIndex = (colorIndex += changeJump);
			bursts[i].index = random(pixelBuffer->getLength());
			bursts[i].step = 0;
			bursts[i].stepRate = 1.0f;
			bursts[i].stepsLeft = random(5) + minBurstLength;
			
			if (oneWay) {
				if (!left && !right) {
					bursts[i].left = random(1) == 0;
					bursts[i].right = !bursts[i].left;
				} else {
					bursts[i].left = left;
					bursts[i].right = right;
				}
			} else {
				bursts[i].left = true;
				bursts[i].right = true;
			}

			break;
		}
	}
}

void LSLPBurst::update() {
	if (fade) pixelBuffer->fade(fadeRate);

	uint16_t lenOffset = (pixelBuffer->getLength() / 40) >> (mirrored ? 1 : 0);

	if (random((minBurstLength >> (oneWay ? 1 : 0)) - lenOffset) == 0) {
		this->addBurst();
	}

	if (mirrored) {
		for (int i = 0; i < MAX_BURSTS; i++) {
			if (bursts[i].stepsLeft == 0)
				continue;

			float lastStep = bursts[i].step - (bursts[i].stepRate / stepChangeRate);
			uint8_t step = (uint8_t)bursts[i].step;

			if ((uint8_t)lastStep != step) {
				uint16_t a = (bursts[i].index + step) % pixelBuffer->getLength();
				uint16_t b = (pixelBuffer->getLength() + bursts[i].index - step) % pixelBuffer->getLength();

				color_t col = colorPalette->getColor(bursts[i].colorIndex);
				float ratio = bursts[i].step / (float)(bursts[i].step + bursts[i].stepsLeft);
				col = fadeColor(col, 1.0 - ratio);

				if (bursts[i].left)  pixelBuffer->setMirroredPixel(a, col);
				if (bursts[i].right) pixelBuffer->setMirroredPixel(b, col);
			}

			bursts[i].step += bursts[i].stepRate;
			bursts[i].stepRate *= stepChangeRate;
			bursts[i].stepsLeft--;
			bursts[i].colorIndex += changeRate;
			bursts[i].colorIndex %= 0x100;
		}
	} else {
		for (int i = 0; i < MAX_BURSTS; i++) {
			if (bursts[i].stepsLeft == 0)
				continue;

			float lastStep = bursts[i].step - (bursts[i].stepRate / stepChangeRate);
			uint8_t step = (uint8_t)bursts[i].step;

			if ((uint8_t)lastStep != step) {
				uint16_t a = (bursts[i].index + step) % pixelBuffer->getLength();
				uint16_t b = (pixelBuffer->getLength() + bursts[i].index - step) % pixelBuffer->getLength();

				color_t col = colorPalette->getColor(bursts[i].colorIndex);
				float ratio = bursts[i].step / (float)(bursts[i].step + bursts[i].stepsLeft);
				col = fadeColor(col, 1.0 - ratio);

				if (bursts[i].left)  pixelBuffer->setPixel(a, col);
				if (bursts[i].right) pixelBuffer->setPixel(b, col);
			}

			bursts[i].step += bursts[i].stepRate;
			bursts[i].stepRate *= stepChangeRate;
			bursts[i].stepsLeft--;
			bursts[i].colorIndex += changeRate;
			bursts[i].colorIndex %= 0x100;
		}
	}

	colorIndex += changeRate;
	LSLightProgram::update();
}
