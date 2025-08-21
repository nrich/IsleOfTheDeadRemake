MKDIR = mkdir -p
CXX = g++
CC = gcc
RM = rm -f
RMDIR = rm -rf
INC = -I src -I thirdparty -I thirdparty/raylib-5.5.0/include -I thirdparty/raylib-cpp-5.5.0 -I thirdparty/fltk-1.4.4/include
LDFLAGS = -static-libgcc -static-libstdc++
CPPFLAGS = -g -O3 -std=c++20 $(INC) -Wall
CFLAGS = -g -O3 $(INC) -Wall -pthread
STRIP = strip

UNAME := $(shell uname -s)

ifeq ($(findstring MINGW, $(UNAME)), MINGW)
    CONFIG_W64=1
endif
 
ifdef CONFIG_W64
    CXX = x86_64-w64-mingw32-g++
    CC = x86_64-w64-mingw32-gcc
    LDFLAGS = -mwindows -static-libgcc -static-libstdc++ -L thirdparty/raylib-5.5.0/lib/mingw-w64 -lraylib -L thirdparty/fltk-1.4.4/lib/mingw-w64 -lgdi32 -lopengl32 -lwinmm -lole32 -luuid -lshell32 -lfltk -lgdiplus -lgdiplus -lole32 -luuid -lcomctl32 -lws2_32 -lwinspool
    INC +=
    CPPFLAGS += -pthread -D_WIN64 -Wall
    ifeq ($(UNAME), Linux)
        WINDRES = x86_64-w64-mingw32-windres
        STRIP = x86_64-w64-mingw32-strip
    else
        WINDRES = windres
        STRIP = strip
    endif
    WINDRESARGS = 
else
    LDFLAGS += -L thirdparty/raylib-5.5.0/lib/x84_64 -lraylib -L thirdparty/fltk-1.4.4/lib/x84_64 -lfltk -pthread -lpthread -lm -ldecor-0 -lwayland-cursor -lwayland-client -lxkbcommon -lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lharfbuzz -lcairo -lX11 -ldl -ldbus-1 -lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 -lharfbuzz -lcairo -lXinerama -lXcursor -lXfixes -lXrender -lX11
endif

VERSION = $(shell cat VERSION.txt)
CPPFLAGS := $(CPPFLAGS) -DVERSION="\"$(VERSION)\""
LDFLAGS := $(LDFLAGS)

# Temporary build directories
ifdef CONFIG_W64
    BUILD := .win64
else
    BUILD := .nix
endif
 
# Define V=1 to show command line.
ifdef V
    Q :=
    E := @true
else
    Q := @
    E := @echo
endif
 
ifdef CONFIG_W64
    TARG := isle_of_the_dead.exe
else
    TARG := isle_of_the_dead
endif
 
all: $(TARG)
 
default: all
 
.PHONY: all default clean strip
 
COMMON_OBJS := \
	src/Animation.o \
	src/CelThree.o \
	src/Entity.o \
	src/Entrance.o \
	src/Flic.o \
	src/Fnt.o \
	src/Help.o \
	src/Inventory.o \
	src/LaunchOptions.o \
	src/Level.o \
	src/Map.o \
	src/Monster.o \
	src/MIDI.o \
	src/MusicPlayer.o \
	src/Palette.o \
	src/Panel.o \
	src/Player.o \
	src/Scene.o \
	src/Segment.o \
	src/SoundCache.o \
	src/StillCel.o \
	src/Strings.o \
	src/Song.o \
	src/TextureCache.o \
	src/Tone.o \
	src/Voc.o \
	src/World.o \
	src/main.o

ifdef CONFIG_W64
OBJS := \
	$(COMMON_OBJS) \
	assets/isle_of_the_dead.res
else
OBJS := \
	$(COMMON_OBJS)
endif
 
# Rewrite paths to build directories
OBJS := $(patsubst %,$(BUILD)/%,$(OBJS))

$(TARG): $(OBJS)
	$(E) [LD] $@    
	$(Q)$(MKDIR) $(@D)
	$(Q)$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	$(E) [CLEAN]
	$(Q)$(RM) $(TARG)
	$(Q)$(RMDIR) $(BUILD)

strip: $(TARG)
	$(E) [STRIP]
	$(Q)$(STRIP) $(TARG)

$(BUILD)/%.o: %.cpp
	$(E) [CXX] $@
	$(Q)$(MKDIR) $(@D)
	$(Q)$(CXX) -c $(CPPFLAGS) -o $@ $<

$(BUILD)/%.o: %.c
	$(E) [CC] $@
	$(Q)$(MKDIR) $(@D)
	$(Q)$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD)/%.res: %.rc
	$(E) [RES] $@
	$(Q)$(MKDIR) $(@D)
	$(Q)$(WINDRES) $< -O coff -o $@ $(WINDRESARGS)
