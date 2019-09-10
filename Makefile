NAME = destruct
VERSION = 0.1
FILES = LICENSE FILE_ID.DIZ README.md
OBJ_CLIENT = olcPixelGameEngine.o client.o vector.o physics.o tank.o \
  game_round.o game_round_update.o game_round_draw.o menu.o
WIN_LIB = libopengl32.a libgdi32.a libgdiplus.a
WIN_DLL = libwinpthread-1.dll

SOURCE = src
OBJ_DIR = obj
TARGET = $(NAME)
LIN_BIN = $(NAME)
WIN_BIN = $(NAME).exe
PACKAGE = $(NAME)-$(VERSION).zip

CXX = g++
WIN_CXX = i686-w64-mingw32-g++
RM = rm -f
COPY = cp -f
MKDIR = mkdir -p
RMDIR = rmdir -p
CPPFLAGS = -g -c -std=c++14
LDFLAGS = -g -pthread -lGL -lX11 -lpng
WIN_LDFLAGS = -g -static-libgcc -static-libstdc++
DCFLAGS = -g -std=c++14 -pthread -lGL -lX11 -lpng
PACK = zip -r

LIN_TARGET = $(TARGET)
WIN_TARGET = $(TARGET)
DIRECTORIES = $(OBJ_DIR) $(TARGET)

WIN_MINGW_DIR = /usr/i686-w64-mingw32/sys-root/mingw
WIN_LIBDIR = $(WIN_MINGW_DIR)/lib

_FILES = $(foreach fn,$(FILES),$(TARGET)/$(fn))
_OBJ_CLIENT = $(foreach obj,$(OBJ_CLIENT),$(OBJ_DIR)/$(obj))
_LIN_BIN = $(foreach bin,$(LIN_BIN),$(LIN_TARGET)/$(bin))
_WIN_BIN = $(foreach bin,$(WIN_BIN),$(WIN_TARGET)/$(bin))
WIN_FILES = $(foreach fn,$(WIN_DLL),$(WIN_TARGET)/$(fn))
WIN_OBJ_CLIENT = $(foreach obj,$(OBJ_CLIENT),$(OBJ_DIR)/$(obj)w)
_WIN_LIB = $(foreach lib,$(WIN_LIB),$(WIN_LIBDIR)/$(lib))

all: linux win32 release

linux: $(DIRECTORIES) $(LIN_TARGET) $(_LIN_BIN) $(_FILES)

win32: $(DIRECTORIES) $(WIN_TARGET) $(_WIN_BIN) $(WIN_FILES) $(_FILES)

release: $(PACKAGE)

$(PACKAGE): $(TARGET)
	$(PACK) $(PACKAGE) $(TARGET)

$(DIRECTORIES):
	$(MKDIR) $(DIRECTORIES)

$(LIN_TARGET)/destruct: $(_OBJ_CLIENT)
	$(CXX) $(_OBJ_CLIENT) -o $@ $(LDFLAGS)

$(TARGET)/destruct.exe: $(WIN_OBJ_CLIENT)
	$(WIN_CXX) $(WIN_OBJ_CLIENT) $(_WIN_LIB) -o $@ $(WIN_LDFLAGS)

$(OBJ_DIR)/%.o: $(SOURCE)/%.cpp
	$(CXX) $< -o $@ $(CPPFLAGS)

$(OBJ_DIR)/%.ow: $(SOURCE)/%.cpp
	$(WIN_CXX) $(CPPFLAGS) $< -o $@

$(WIN_TARGET)/%.dll: $(WIN_MINGW_DIR)/bin/%.dll
	$(COPY) $< $@

$(TARGET)/%: %
	$(COPY) $< $@

clean:
	$(RM) *.o
	$(RM) *.o32
	$(RM) $(PACKAGE)
	$(RM) $(TARGET)/*
	$(RM) $(OBJ_DIR)/*
	$(RMDIR) $(DIRECTORIES)
#	$(RMDIR) $(LIN_TARGET) $(WIN_TARGET) $(DIRECTORIES)

