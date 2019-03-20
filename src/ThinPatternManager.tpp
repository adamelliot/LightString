// -------------------- Thin Pattern Manager --------------------

template <template <typename> class PIXEL, typename FORMAT>
bool ThinPatternManager<PIXEL, FORMAT>::update() {
	uint32_t time = millis(), timeDelta = time - lastTime;

	// TODO: Frame rate limiting isn't working quite right
	if (timeDelta < msPerFrame) {
		usleep(1000);
		// delay(msPerFrame - timeDelta);
		return false;
	}
	lastTime = time;

	LightLayer<PIXEL, FORMAT>::update();
#ifdef USE_FASTLED
	FastLED.setBrightness(this->getOpacity());
#endif
	
	return true;
}
