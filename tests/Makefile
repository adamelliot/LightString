CC=g++
CFLAGS=-c -Wall -I/Users/adam/Code/Arduino/libraries/FastLED
LDFLAGS=-lstdc++ -lCppUnitLite
SOURCES=fakes/FakeSerial.cpp colortypes_tests.cpp tests.cpp
OBJECTS=$(addsuffix .o, $(addprefix .build/, $(basename $(SOURCES))))
EXECUTABLE=test

# -I/Applications/Arduino.app/Contents/Java/hardware/tools/avr/avr/include -I/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino -I/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/variants/standard  

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm *.o
	rm fakes/*.o
	rm $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	./$(EXECUTABLE)

.build/%.o: %.cpp
	mkdir -p .build/$(dir $<)
	$(CC) $(CFLAGS) $< -o $@