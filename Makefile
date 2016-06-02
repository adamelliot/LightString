APP_NAME = lightstring

CC = g++
CFLAGS = -c -Wall -std=c++14 -O0 -g -DDEBUG
#-I/Users/adam/Code/Arduino/libraries/FastLED
LDFLAGS = 

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(addsuffix .o, $(addprefix .build/, $(basename $(SOURCES))))

TEST_CFLAGS = -Igoogle-test
TEST_CORE_SOURCES = google-test/src/gtest-all.cc google-test/src/gmock-all.cc
TEST_SOURCES = $(TEST_CORE_SOURCES) $(wildcard tests/*.cpp) src/LightProgram.cpp src/utils.cpp
TEST_OBJECTS = $(addsuffix .o, $(addprefix .build/, $(basename $(TEST_SOURCES))))

# -I/Applications/Arduino.app/Contents/Java/hardware/tools/avr/avr/include -I/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino -I/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/variants/standard  

all: $(OBJECTS)

clean:
	rm -rf .build
	rm $(APP_NAME)_tests

test: $(TEST_OBJECTS)
	$(CC) $(LDFLAGS) $(TEST_OBJECTS) -o $(APP_NAME)_tests
	./$(APP_NAME)_tests

.build/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $< -o $@

.build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $< -o $@

.build/src/%.o: %.cpp
	mkdir -p .build/$(dir $<)
	$(CC) $(CFLAGS) $< -o $@

# GTest

# Make the required gtest files tests

# gtest-all.o: gtest-all.cc
#     @mkdir -p .build
#     $(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o .build/$@
# 
# gtest_main.o: gtest_main.cc
#     @mkdir -p .build
#     $(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o .build/$@
# 
# gtest.a: gtest-all.o
#     $(AR) $(ARFLAGS) $@ $^
# 
# gtest_main.a: gtest-all.o gtest_main.o
#     $(AR) $(ARFLAGS) $@ $^
