#ifndef GAME_H
#define GAME_H

#include <set>
#include <map>
#include "util.h"
#include "vector.h"
#include "players.h"

// Constant limits
const int MAX_PLAYERS = 16;

// Number of physics rounds between screen draws
extern int PHYSICS_ROUNDS;

// Lists for random names and quotes
extern std::vector<std::string> npc_names;
extern std::vector<std::string> quotes_shoot;
extern std::vector<std::string> quotes_death;

// Set to store map columns where ground may have been changed
extern std::set<int> changed;
// Range where columns did change. This will be cleared when mab has been
// cleaned on screen
extern std::map<int, Range> columns;

// Struct to hold game properties
struct Configuration {
  enum WallType { NONE, CONCRETE, RUBBER, WARP, RANDOM };

  // Game properties
  int rounds = 4;
  int player_count = 2;
  Player::Type default_player_type = Player::HUMAN;
  // Map properties
  int width = 400, height = 300;
  WallType wall = NONE;
  bool wind = true, changing_wind = true;
  Vector2D gravity = { 0.0, 5.0 };
  // Scoring values
  int points_winning = 1000;
  int points_kill = 200;
  int points_suicide = -250;
  Fraction points_damage = { 1, 1 }; // 1
  Fraction points_self_damage = { -1, 2 }; // -0.5

  // List for players
  std::vector<Player*> players;

  Configuration();
  ~Configuration();

  void Init();
};

extern Configuration configuration;

struct GameRound {
  enum State { PREPARATION, SHOOT, SHOOTING, ENDING, SCORES, END };

  // Game configuration
  const Configuration* config;
  // Map settings
  int width = 400, height = 300;
  Configuration::WallType wall = Configuration::NONE;
  int min = 100, max = 280;
  int initial_jump = 512;
  // Tanks for players
  std::vector<Tank*> tanks;
  Tank* selected_tank = NULL;
  // Phycis values
  Vector2D gravity = { 0.0, 5.0 }, wind = { 0.0, 0.0 };
  // Round properties
  int round = 0, tic = 0;
  // Round state
  State state = PREPARATION, prev_state = PREPARATION;

  // Switch for redrawing screen
  bool refresh = true;

  GameRound(const Configuration*, const int);

  ~GameRound();

  bool CreateRound();

  bool Update();

private:
  bool InsertTanks();

  bool UpdateParticles();

  bool UpdatePreparation();
  bool UpdateShoot();
  bool UpdateShooting();
  bool UpdateEnding();
  bool UpdateScores();
};

extern GameRound* game_round;

bool Game_Init();
bool Game_Prepare();

uint8_t Map_IsGround(int, int);
uint32_t Map_GetColor(int, int);
void Map_ClearPixel(int, int);
void Map_ClearCircle(int, int, int);
void Map_MovePixel(int, int, int, int);
void Map_CreateMap(GameRound*);
void Map_DestroyMap();

bool DrawRound_Draw();
bool DrawRound_Clear();
bool DrawRound_Interface(Tank*);
bool DrawRound_Scores();

#endif

