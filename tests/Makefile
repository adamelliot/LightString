SOURCES = fake_Serial.cpp runtests.cpp colortypes_tests.cpp

#OBJECTS := $(addsuffix .o, $(addprefix .build/, $(basename $(SOURCES))))
#DEPFILES := $(subst .o,.dep, $(subst .build/,.deps/, $(OBJECTS)))
#TESTCPPFLAGS = -D_TEST_ -Ilibraries/dsm2_tx -Itest -Iarduino
#CPPDEPFLAGS = -MMD -MP -MF .deps/$(basename $<).dep
RUNTESTS := runtests
#
#all: runtests
#
#.build/%.o: %.cpp
#	mkdir -p .deps/$(dir $<)
#	mkdir -p .build/$(dir $<)
#	$(COMPILE.cpp) $(TESTCPPFLAGS) $(CPPDEPFLAGS) -o $@ $<

runtests: $(OBJECTS)
	$(CC) -o $(RUNTESTS) $(SOURCES) -lstdc++ -lCppUnitLite -DARDUINO -I/Users/adam/Code/Arduino/libraries/FastLED
# -I/Applications/Arduino.app/Contents/Java/hardware/tools/avr/avr/include -I/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino -I/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/variants/standard  

test: clean runtests
	@./$(RUNTESTS)

clean:
	@rm -rf $(RUNTESTS)
