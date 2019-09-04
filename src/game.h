#ifndef GAME_H
#define GAME_H

#include "olcPixelGameEngine.h"
#include "vector.h"

const int MAX_PLAYERS = 16;
const int MIN_ANGLE = -100, MAX_ANGLE =  100;
const int MAX_POWER = 1000;

enum PlayerType { NONE, HUMAN, RANDOM };

struct Area {
  int x, y, w, h;

  Area(int, int, int, int);
};

struct Player {
  PlayerType type;
  olc::Pixel color;

  Player(olc::Pixel, PlayerType);
};

struct Tank {
  Player* player = NULL;
  Point loc = { -1, -1 };
  int angle = 0, power = 200;
  int health = 100, fall = 0;
  bool alive = true;
  bool ready = false;

  Tank();
  Tank(Player*);

  bool Prepare(olc::PixelGameEngine*);
  bool Update();
  Area GetArea();
};

class GameRound {
public:
  int width, height;
  int min = 25, max;
  int initial_jump = 512;
  olc::Sprite* background = NULL;
  olc::Sprite* ground = NULL;
  // Tanks for players
  Tank tanks[MAX_PLAYERS];
  int tank_count = 0, selected = -1;
  // Phycis objects
  Vector2D gravity = { 0.0, 0.05 };
  int tic = 0;
  // Switch to refresh screen
  bool refresh = true;

  // Round state
  enum State { PREPARATION, SHOOT, SHOOTING, ENDING, END };
  State state = PREPARATION;

  GameRound(int, int, std::vector<Player*>);

  ~GameRound();

  bool CreateRound();

  bool Update(olc::PixelGameEngine*);

  bool ClearRound(olc::PixelGameEngine*);

  bool DrawRound(olc::PixelGameEngine*);

private:
  bool CreateMap();

  bool InsertTanks();

  bool ClearArea(olc::PixelGameEngine*, Area);

  bool UpdatePreparation(olc::PixelGameEngine*);
  bool UpdateShoot();
  bool UpdateShooting();
  bool UpdateEnding();
};

#endif
