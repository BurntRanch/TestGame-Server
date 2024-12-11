CXX       	?= g++
PREFIX	  	?= /usr
VARS  	  	?=

DEBUG		?= 1
# https://stackoverflow.com/a/1079861
# WAY easier way to build debug and release builds
ifeq ($(DEBUG), 1)
        BUILDDIR  = build/debug
        CXXFLAGS := -g -Wpedantic -Wall -Wextra -Wno-unused-parameter $(DEBUG_CXXFLAGS) $(CXXFLAGS)
else
        BUILDDIR  = build/release
        CXXFLAGS := -O2 $(CXXFLAGS)
endif

NAME		 = LostInTransit
TARGET		 = game
SRC 	   	 = $(sort $(wildcard src/*.cpp))
OBJ 	   	 = $(SRC:.cpp=.o)
LDFLAGS   	+= -L$(BUILDDIR)/sdl_engine -L$(BUILDDIR)/sdl_engine/steam -lfmt -lSDL3 -lengine -lGameNetworkingSockets
CXXFLAGS  	?= -mtune=generic -march=native
CXXFLAGS        += -funroll-all-loops -Iinclude -Ilib/sdl_engine/include -isystem/usr/include/freetype2 -std=c++17 $(VARS)

all: engine $(TARGET)

$(TARGET): engine $(OBJ)
	mkdir -p $(BUILDDIR)
	$(CXX) $(OBJ) -o $(BUILDDIR)/$(TARGET) $(LDFLAGS)

engine:
ifeq ($(wildcard $(BUILDDIR)/sdl_engine/libengine.so.1),)
	make -C lib/sdl_engine CXX=$(CXX) VARS=$(VARS) DEBUG=$(DEBUG)
	mkdir -p $(BUILDDIR)/sdl_engine
	mv lib/sdl_engine/$(BUILDDIR)/libengine.so.1 $(BUILDDIR)/sdl_engine/
	mv lib/sdl_engine/$(BUILDDIR)/steam/ $(BUILDDIR)/sdl_engine/
	cd $(BUILDDIR)/sdl_engine && ln -sf libengine.so.1 libengine.so
endif

dist: $(TARGET)
	bsdtar -zcf $(NAME)-v$(VERSION).tar.gz LICENSE README.md -C $(BUILDDIR) $(TARGET)

clean:
	rm -rf $(BUILDDIR)/* $(OBJ)

distclean:
	rm -rf $(BUILDDIR) $(OBJ)

.PHONY: $(TARGET) distclean dist clean all
