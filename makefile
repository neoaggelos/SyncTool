# Change the following to suit your environment
CXX=g++
CXXFLAGS+=
LIBS+=
PREFIX=/usr/local

# Do not change anything below this line
SOURCES=sources/main.cpp sources/util.cpp sources/synctool.cpp
SOURCES_WIN=sources/windows.cpp $(SOURCES)
SOURCES_LNX=sources/dirent.cpp $(SOURCES)

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
