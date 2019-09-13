#ifndef GAME_H
#define GAME_H

#include <set>
#include <map>
#include "util.h"
#include "vector.h"
#include "olcPixelGameEngine.h"

const int MAX_PLAYERS = 16;
const int MAX_HEALTH = 100;
const int MIN_ANGLE = -95, MAX_ANGLE =  95;
const int MIN_POWER = 10, MAX_POWER = 1500;

const int POINTS_WINNER = 1000;
const int POINTS_KILL = 100;
const int POINTS_SUICIDE = -750;
//const int POINTS_ = ;

enum PlayerType { NONE, HUMAN, RANDOM };

extern int INITIAL_PLAYERS;
extern PlayerType DEFAULT_PLAYER_TYPE;
extern int PHYSICS_ROUNDS;

struct Interface { int angdir = 0, powdir = 0; bool ready = false; };

struct Player {
  PlayerType type;
  std::string name = "[no name]";
  uint32_t color;
  int64_t score = 0;

  Player(std::string, uint32_t, PlayerType);
  void SwitchType();
};

extern std::vector<Player*> players;
extern int game_rounds;

extern std::vector<std::string> npc_names;
extern std::vector<std::string> quotes_shoot;
extern std::vector<std::string> quotes_death;

bool Init();

bool PrepareGame();

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

struct GameRound {
  int width, height;
  int min = 200, max;
  int initial_jump = 512;
  // Ground and background images
  olc::Sprite* ground = NULL;
  // Variables for dropping ground
  std::set<int> changed;
  std::map<int, Range> columns;
  // Tanks for players
  std::vector<Tank*> tanks;
  Tank* selected_tank = NULL;
  // Phycis objects
  Vector2D gravity = { 0.0, 5.0 }, wind = { 0.0, 0.0 };
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

private:
  bool CreateMap();

  bool InsertTanks();

  bool UpdateParticles();

  bool UpdatePreparation(const Interface);
  bool UpdateShoot();
  bool UpdateShooting();
  bool UpdateEnding();
  bool UpdateScores(const Interface);
};

#endif

