APP_NAME = wheel

LIGHTSTRING_ROOT = $(abspath ../..)
LIGHTSTRING_SOURCES = $(wildcard $(LIGHTSTRING_ROOT)/src/*.cpp)

CC = g++
CFLAGS = -Wall -g -O0 -c -std=c++14 -I$(LIGHTSTRING_ROOT)/src
LDFLAGS = 
SOURCES = $(LIGHTSTRING_SOURCES) $(wildcard *.cpp)
OBJECTS = $(addsuffix .o, $(addprefix .build/, $(basename $(SOURCES))))

all: $(SOURCES) $(APP_NAME)

run: all
	./$(APP_NAME)

debug: all
	lldb ./$(APP_NAME)

clean:
	rm -rf .build
	rm $(APP_NAME)

$(APP_NAME): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.build/%.o: %.cpp $(DEPS)
	@mkdir -p .build/$(dir $<)
	$(CC) $(CFLAGS) $< -o $@
