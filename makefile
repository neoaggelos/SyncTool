# Change the following to suit your environment
CXX=g++
CXXFLAGS+=
PREFIX=/usr/local

# Do not change anything below this line
SOURCES=sources/util.cpp sources/main.cpp sources/synctool.cpp
SOURCES_WIN=$(SOURCES) sources/windows.cpp
SOURCES_LNX=$(SOURCES) sources/dirent.cpp

all: synctool

.phony: install clean windows

synctool: $(SOURCES_LNX)
	$(CXX) $(SOURCES_LNX) -o synctool

windows: $(SOURCES_WIN)
	$(CXX) $(SOURCES_WIN) -o synctool.exe

install: synctool
	install -c synctool $(PREFIX)/bin/synctool

clean:
	rm -rf synctool build
