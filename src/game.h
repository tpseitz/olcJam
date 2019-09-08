#ifndef GAME_H
#define GAME_H

#include "olcPixelGameEngine.h"
#include "vector.h"

const int MAX_PLAYERS = 16;
const int MAX_HEALTH = 100;
const int MIN_ANGLE = -95, MAX_ANGLE =  95;
const int MIN_POWER = 10, MAX_POWER = 1500;

const int POINTS_WINNER = 1000;
const int POINTS_KILL = 100;
const int POINTS_SUICIDE = -750;
//const int POINTS_ = ;

enum PlayerType { NONE, HUMAN, RANDOM };

struct Area {
  int x, y, w, h;

  Area(int, int, int, int);
};

struct Interface { int angdir = 0, powdir = 0; bool ready = false; };

struct Player {
  PlayerType type;
  std::string name = "[no name]";
  olc::Pixel color;
  int64_t score = 0;

  Player(std::string, olc::Pixel, PlayerType);
  void SwitchType();
};

extern std::vector<Player*> players;
extern int game_rounds;

struct Tank {
  Player* player = NULL;
  Point loc = { -1, -1 };
  int angle = 0, power = 200;
  int health = MAX_HEALTH, fall = 0;
  bool alive = true;
  bool ready = false;

  Tank();
  Tank(Player*);

  bool Prepare(const Interface);
  bool Update();
  void CheckLimits();
  Area GetArea();
};

olc::Sprite* DiamondSquare(int, int);

class GameRound {
public:
  int width, height;
  int min = 25, max;
  int initial_jump = 512;
  olc::Sprite* background = NULL;
  olc::Sprite* ground = NULL;
  // Tanks for players
  std::vector<Tank*> tanks;
  Tank* selected_tank = NULL;
  // Phycis objects
  Vector2D gravity = { 0.0, 0.05 };
  int rounds_left = 0, tic = 0;
  // Switch to refresh screen
  bool refresh = true;
  // Round state
  enum State { PREPARATION, SHOOT, SHOOTING, ENDING, SCORES, END };
  State state = PREPARATION, prev_state = PREPARATION;

  GameRound(int, int, std::vector<Player*>, int);

  ~GameRound();

  bool CreateRound();

  bool Update(const Interface);

  bool ClearRound(olc::PixelGameEngine*);

  bool DrawRound(olc::PixelGameEngine*);

private:
  bool CreateMap();

  bool InsertTanks();

  bool ClearArea(olc::PixelGameEngine*, Area);

  bool DrawInterface(olc::PixelGameEngine*, Tank*);

  bool DrawScores(olc::PixelGameEngine*);

  bool UpdatePreparation(const Interface);
  bool UpdateShoot();
  bool UpdateShooting();
  bool UpdateEnding();
  bool UpdateScores(const Interface);
};

#endif
