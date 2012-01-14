
PREF = /usr/local/mingw32ce

TARGET = xclannad.exe

DEFS = -DWINCE -DWIN32 -DENABLE_PATCH
CFLAGS = $(DEFS) -O2 -Wall -fsigned-char -DHAVE_CONFIG_H -Drandom=rand -Dmain=SDL_main $(shell $(PREF)/bin/sdl-config --cflags) $(shell $(PREF)/bin/freetype-config --cflags)

LDFLAGS = -static `$(PREF)/bin/sdl-config --libs` -lSDL_mixer -lSDL -lvorbisidec -lmad -lfreetype -ljpeg -lpng -lz

include Makefile.xclannad

OBJS = $(XCLANNAD_OBJS)
CFLAGS += $(XCLANNAD_CFLAGS)

all : $(TARGET)

CC = $(PREF)/bin/arm-wince-mingw32ce-gcc.exe
CXX= $(PREF)/bin/arm-wince-mingw32ce-g++.exe
LD = $(PREF)/bin/arm-wince-mingw32ce-g++.exe

xclannad.exe : $(OBJS)
	$(LD) -o xclannad.exe $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

.cc.o:
	$(CXX) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

