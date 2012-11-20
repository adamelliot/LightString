#include "LSLP2DLines.h"

LSLightProgram *factory2DLines(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLP2DLines(pixelBuffer, colorPalette, colorFunc);
}

LSLP2DLines::LSLP2DLines(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	pixelBuffer->setSize(22, 14);
	
	fade = true;//random(3) != 2;

	fadeRate = 0.4 + ((float)random(201) / 1000);
	
	colorIndex = random(0xff);
	changeRate = random(8) + 3;
	
	x0 = random(pixelBuffer->getWidth());
	y0 = random(pixelBuffer->getHeight());
	x1 = random(pixelBuffer->getWidth());
	y1 = random(pixelBuffer->getHeight());
  x2 = random(pixelBuffer->getWidth());
	y2 = random(pixelBuffer->getHeight());
  x3 = random(pixelBuffer->getWidth());
	y3 = random(pixelBuffer->getHeight());
  
  float base = 0.0, vel = 0.4;
	sx0 = vel * (float)random(32767) / 32767.0 + base;
	sy0 = vel * (float)random(32767) / 32767.0 + base;
  sx1 = vel * (float)random(32767) / 32767.0 + base;
	sy1 = vel * (float)random(32767) / 32767.0 + base;
  sx2 = vel * (float)random(32767) / 32767.0 + base;
	sy2 = vel * (float)random(32767) / 32767.0 + base;
  sx3 = vel * (float)random(32767) / 32767.0 + base;
	sy3 = vel * (float)random(32767) / 32767.0 + base;
}

void LSLP2DLines::update() {
	if (fade) pixelBuffer->fade(fadeRate);

  pixelBuffer->drawLine(x0, y0, x1, y1, colorPalette->getColor(colorIndex));
  pixelBuffer->drawLine(x2, y2, x3, y3, colorPalette->getColor(colorIndex + 60));

  x0 += sx0; y0 += sy0;
  x1 += sx1; y1 += sy1;
  x2 += sx2; y2 += sy2;
  x3 += sx3; y3 += sy3;
  
  if (x0 >= pixelBuffer->getWidth()) {
    x0 = pixelBuffer->getWidth();
    sx0 *= -1.0;
  } else if (x0 < 0.0) {
    x0 = 0.0;
    sx0 *= -1.0;
  }
  if (y0 >= pixelBuffer->getHeight()) {
    y0 = pixelBuffer->getHeight();
    sy0 *= -1.0;
  } else if (y0 < 0.0) {
    y0 = 0.0;
    sy0 *= -1.0;
  }

  if (x1 >= pixelBuffer->getWidth()) {
    x1 = pixelBuffer->getWidth();
    sx1 *= -1.0;
  } else if (x1 < 0.0) {
    x1 = 0.0;
    sx1 *= -1.0;
  }
  if (y1 >= pixelBuffer->getHeight()) {
    y1 = pixelBuffer->getHeight();
    sy1 *= -1.0;
  } else if (y1 < 0.0) {
    y1 = 0.0;
    sy1 *= -1.0;
  }

  if (x2 >= pixelBuffer->getWidth()) {
    x2 = pixelBuffer->getWidth();
    sx2 *= -1.0;
  } else if (x2 < 0.0) {
    x2 = 0.0;
    sx2 *= -1.0;
  }
  if (y2 >= pixelBuffer->getHeight()) {
    y2 = pixelBuffer->getHeight();
    sy2 *= -1.0;
  } else if (y2 < 0.0) {
    y2 = 0.0;
    sy2 *= -1.0;
  }

  if (x3 >= pixelBuffer->getWidth()) {
    x3 = pixelBuffer->getHeight();
    sx3 *= -1.0;
  } else if (x3 < 0.0) {
    x3 = 0.0;
    sx3 *= -1.0;
  }
  if (y3 >= pixelBuffer->getHeight()) {
    y3 = pixelBuffer->getHeight();
    sy3 *= -1.0;
  } else if (y3 < 0.0) {
    y3 = 0.0;
    sy3 *= -1.0;
  }

	colorIndex += changeRate;
	LSLightProgram::update();
}
