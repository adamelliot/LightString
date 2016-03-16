#pragma once

#include <iostream>

class FakeSerial {
public:
  void begin(unsigned long);
  void end();
	void print(const char *str = 0);
	void println(const char *str = 0);
};

extern FakeSerial Serial;