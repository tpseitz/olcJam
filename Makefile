CXX = g++
RM = rm -f
COPY = cp -n
MKDIR = mkdir -p
RMDIR = rmdir -p
CPPFLAGS = -g -c -std=c++14
LDFLAGS = -g -pthread -lGL -lX11 -lpng
DCFLAGS = -g -std=c++14 -pthread -lGL -lX11 -lpng
PACK = zip -r

SOURCE = src
TARGET = destruct
DIRECTORIES = $(TARGET)

FILES = LICENSE FILE_ID.DIZ README.md

OBJ_CLIENT = olcPixelGameEngine.o client.o vector.o physics.o tank.o \
  game_round.o game_round_update.o game_round_draw.o menu.o

all: client files $(TARGET).zip

client: $(DIRECTORIES) $(OBJ_CLIENT)
	$(CXX) $(OBJ_CLIENT) -o $(TARGET)/$@ $(LDFLAGS)

%.o: $(SOURCE)/%.cpp
	$(CXX) $< -o $@ $(CPPFLAGS)

%.zip: %
	$(PACK) $@ $<

$(DIRECTORIES):
	$(MKDIR) $(DIRECTORIES)

files:
	$(COPY) -t$(TARGET) $(FILES)

clean:
	$(RM) *.o
	$(RM) *.zip
	$(RM) $(TARGET)/*
	$(RMDIR) $(DIRECTORIES)

