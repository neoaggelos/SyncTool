### CHANGE THE FOLLOWING VALUES TO MUCH YOUR PREFERENCES ###

CXX=g++
PREFIX=/usr/local

### ADD HERE ANY LIBRARIES THAT MIGHT BE NEEDED FOR YOUR SYSTEM ###

LIBS=

### UNCOMMENT THE FOLLOWING LINE FOR A DEBUG BUILD ###

#CXXFLAGS+=-g -D_DEBUG 


### DO NOT CHANGE ANYTHING BELOW THIS LINE ###

SOURCES=$(shell find ./sources | grep cpp | grep -v "windows.cpp")
OBJECTS=$(shell echo $(SOURCES) | sed -e "s,\.cpp,\.o,g")
TARGET=synctool

CXXFLAGS+=-Wall -pedantic

all: prepare $(TARGET)

$(TARGET): prepare $(OBJECTS)
	$(CXX) $(LIBS) $(OBJECTS) -o $(TARGET)

prepare:
	if [ ! -d obj ]; then mkdir obj; fi

clean:
	rm -rf $(OBJECTS) obj $(TARGET) build bin

install:
	if [ ! -d "$(PREFIX)/bin" ]; then mkdir -p "$(PREFIX)/bin"; fi
	install $(TARGET) "$(PREFIX)/bin/$(TARGET)"
	chmod 0775 "$(PREFIX)/bin/$(TARGET)"
