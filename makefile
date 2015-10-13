# Change the following to suit your environment
CXX=g++
CXXFLAGS+=
LIBS+=
PREFIX=/usr/local
TARGET=synctool

# Do not change anything below this line
SOURCES=$(wildcard sources/*.cpp)

all: synctool

.phony: install clean windows

synctool: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LIBS) $(SOURCES) -o $(TARGET)

windows: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LIBS) $(SOURCES) -o $(TARGET).exe

install: synctool
	install -c synctool $(PREFIX)/bin/synctool

clean:
	rm -rf synctool synctool.exe build
