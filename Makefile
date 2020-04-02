NAME = destruct
VERSION = 0.2
DOCS = LICENSE FILE_ID.DIZ README.md
OBJ_CLIENT = olcPixelGameEngine.o client_pge.o vector.o physics.o tank.o \
  util.o game_round.o game_round_update.o particles.o game_round_draw.o menu.o
WIN_LIB = libopengl32.a libgdi32.a libgdiplus.a
WIN_DLL = libwinpthread-1.dll
TESTS = strip read

SOURCE = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data
LIN_BIN = $(NAME)
WIN_BIN = $(NAME).exe
PACKAGE = $(NAME)-$(VERSION)

CXX = g++
WIN_CXX = i686-w64-mingw32-g++
RM = rm -f
COPY = cp -f
MKDIR = mkdir -p
RMDIR = rmdir -p
CPPFLAGS = -g -c -std=c++14
LDFLAGS = -g -pthread -lGL -lX11 -lpng -lstdc++fs
WIN_LDFLAGS = -g -static-libgcc -static-libstdc++
DCFLAGS = -g -std=c++14 -pthread -lGL -lX11 -lpng
PACK = zip -r

DIRECTORIES = $(OBJ_DIR) $(BIN_DIR)
TEST_DIR = tests

WIN_MINGW_DIR = /usr/i686-w64-mingw32/sys-root/mingw
WIN_LIBDIR = $(WIN_MINGW_DIR)/lib

_OBJ_CLIENT = $(foreach obj,$(OBJ_CLIENT),$(OBJ_DIR)/$(obj))
_LIN_BIN = $(foreach bin,$(LIN_BIN),$(BIN_DIR)/$(bin))
_WIN_BIN = $(foreach bin,$(WIN_BIN),$(BIN_DIR)/$(bin))
_WIN_DLL = $(foreach fn,$(WIN_DLL),$(BIN_DIR)/$(fn))
WIN_OBJ_CLIENT = $(foreach obj,$(OBJ_CLIENT),$(OBJ_DIR)/$(obj)w)
_WIN_LIB = $(foreach lib,$(WIN_LIB),$(WIN_LIBDIR)/$(lib))
_TESTS = $(foreach test,$(TESTS),$(TEST_DIR)/test_$(test))

TESTFLAGS = -g -std=c++14

all: linux win32 release

linux: $(DIRECTORIES) $(_LIN_BIN)

win32: $(DIRECTORIES) $(_WIN_BIN) $(_WIN_DLL)

release:
	$(PACK) $(PACKAGE).zip $(BIN_DIR) $(DATA_DIR) $(DATA_DIR) $(DOCS)

$(DIRECTORIES):
	$(MKDIR) $@

$(BIN_DIR)/destruct: $(_OBJ_CLIENT)
	$(CXX) $(_OBJ_CLIENT) -o $@ $(LDFLAGS)

$(BIN_DIR)/destruct.exe: $(WIN_OBJ_CLIENT)
	$(WIN_CXX) $(WIN_OBJ_CLIENT) $(_WIN_LIB) -o $@ $(WIN_LDFLAGS)

$(OBJ_DIR)/%.o: $(SOURCE)/%.cpp
	$(CXX) $< -o $@ $(CPPFLAGS)

$(OBJ_DIR)/%.ow: $(SOURCE)/%.cpp
	$(WIN_CXX) $(CPPFLAGS) $< -o $@

$(BIN_DIR)/%.dll: $(WIN_MINGW_DIR)/bin/%.dll
	$(COPY) $< $@

tests: $(_TESTS)

$(TEST_DIR)/test_%: $(TEST_DIR)/%.cpp
	$(CXX) $< -o $@ $(TESTFLAGS)

clean:
	$(RM) *.o
	$(RM) *.o32
	$(RM) $(PACKAGE).zip
	$(RM) $(BIN_DIR)/*
	$(RM) $(OBJ_DIR)/*
	$(RM) $(_TESTS)
	$(RMDIR) $(DIRECTORIES)

