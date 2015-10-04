# Change the following to suit your environment
CXX=g++
CXXFLAGS+=
LIBS+=
PREFIX=/usr/local

# Do not change anything below this line
SOURCES=$(wildcard sources/*.cpp)
SOURCES_WIN=$(filter-out sources/dirent.cpp, $(SOURCES))
SOURCES_LNX=$(filter-out sources/windows.cpp, $(SOURCES))

all: synctool

.phony: install clean windows

synctool: $(SOURCES_LNX)
	$(CXX) $(CXXFLAGS) $(LIBS) $(SOURCES_LNX) -o synctool

windows: $(SOURCES_WIN)
	$(CXX) $(CXXFLAGS) $(LIBS) $(SOURCES_WIN) -o synctool.exe

install: synctool
	install -c synctool $(PREFIX)/bin/synctool

clean:
	rm -rf synctool synctool.exe build
