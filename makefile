# Change the following to suit your environment
CXX=g++
CXXFLAGS+=
LIBS+=
PREFIX=/usr/local
TARGET=synctool

# Do not change anything below this line
SOURCES=sources/linux/*.cpp
WIN_SOURCES=sources/windows/*.cpp

all: $(TARGET)

.phony: install clean windows

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LIBS) -o $(TARGET)

windows: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(WIN_SOURCES) $(LIBS) -o $(TARGET).exe

install: $(TARGET)
	install -c $(TARGET) $(PREFIX)/bin/$(TARGET)

clean:
	rm -rf $(TARGET) $(TARGET).exe build
