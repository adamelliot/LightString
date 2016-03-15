#include <cstring>
#include <iostream>
#include <iomanip>

#include "fake_Serial.h"

void FakeSerial::begin(unsigned long speed) {
  return;
}

void FakeSerial::end() {
  return;
}

void FakeSerial::print(const char *str) {
  using namespace std;

	cout << str;
}

void FakeSerial::println(const char *str) {
  using namespace std;

	cout << str << endl;
}

FakeSerial Serial;