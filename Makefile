CXX = g++
RM = rm -f
MKDIR = mkdir -p
RMDIR = rmdir -p
CPPFLAGS = -g -c -std=c++14
LDFLAGS = -g -pthread -lGL -lX11 -lpng
DCFLAGS = -g -std=c++14 -pthread -lGL -lX11 -lpng

SOURCE = src
TARGET = bin
DIRECTORIES = $(TARGET)

OBJ_CLIENT = olcPixelGameEngine.o client.o game_round.o game_round_update.o \
  vector.o physics.o

all: client

client: $(DIRECTORIES) $(OBJ_CLIENT)
	$(CXX) $(OBJ_CLIENT) -o $(TARGET)/$@ $(LDFLAGS)

%.o: $(SOURCE)/%.cpp
	$(CXX) $< -o $@ $(CPPFLAGS)

$(DIRECTORIES):
	$(MKDIR) $(DIRECTORIES)

clean:
	$(RM) *.o
	$(RM) $(TARGET)/*
	$(RMDIR) $(DIRECTORIES)

