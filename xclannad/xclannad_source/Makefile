
PSPPRE = $(shell psp-config --psp-prefix)
PSPDEV = $(shell psp-config --pspdev-path)
PSPSDK = $(shell psp-config --pspsdk-path)

TARGET = xclannad

DEFS = -DPSP -DENABLE_PATCH
CFLAGS = $(DEFS) -O2 -G0 -Wall -Drandom=rand -DHAVE_CONFIG_H $(shell $(PSPPRE)/bin/sdl-config --cflags) $(shell $(PSPPRE)/bin/freetype-config --cflags)

LDFLAGS = 
LIBS = -lSDL_ttf -lSDL_image -lSDL_mixer -lSDL -lSDLmain -lfreetype -lbz2 -ljpeg -lpng -lz -lmad -lvorbisidec -lm -lstdc++ -lGL -lpspvfpu -lpspdebug -lpspgu -lpspctrl -lpspge -lpspdisplay -lpsphprm -lpspsdk -lpsprtc -lpspaudio

PSP_FW_VERSION = 371
PSP_LARGE_MEMORY = 1
USE_USER_LIBS = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = xlove生 for PSP 0.07h

include Makefile.xclannad

OBJS = $(XCLANNAD_OBJS)
CFLAGS += $(XCLANNAD_CFLAGS)

include $(PSPSDK)/lib/build.mak

