#include "game.h"
#include "physics.h"

// Variables for dropping ground
std::set<int> changed;
std::map<int, Range> columns;

int INITIAL_PLAYERS = 2;
PlayerType DEFAULT_PLAYER_TYPE = HUMAN;

std::vector<Player*> players;
// Quote lists
std::vector<std::string> npc_names;
std::vector<std::string> quotes_shoot;
std::vector<std::string> quotes_death;

int game_rounds = 5;

Player::Player(std::string name, uint32_t col, PlayerType tp) {
  score = 0;
  this->name = name;
  color = col;
  type = tp;
}

void Player::SwitchType() {
  switch (type) {
    case HUMAN: type = RANDOM; break;
    case RANDOM: type = HUMAN; break;
  }
}

bool Init() {
  npc_names    = ReadLines("data/names.txt");
  quotes_shoot = ReadLines("data/shoot.txt");
  quotes_death = ReadLines("data/death.txt");

  for (int i = 0; i < INITIAL_PLAYERS; i++)
    players.push_back(new Player("Player " + std::to_string(i + 1),
      COLOR_LIST[i % COLOR_COUNT], DEFAULT_PLAYER_TYPE));

  return true;
}

bool PrepareGame() {
  std::set<int> names_exclude;
  for (Player* plr: players) {
    if (plr->type == RANDOM)
      plr->name = RandomChoice(npc_names, &names_exclude);
    plr->score = 0;
  }

  return true;
}

GameRound::GameRound(int w, int h, std::vector<Player*> players, int rounds) {
  width = w;
  height = h;
  rounds_left = rounds;
  max = height - 20;

  wind = { (float)((double)((rand() % 1000) - 500) / 1000.0), 0.0 };

  int ndx = 0;
  while (players.size() > 0) {
    int i = rand() % players.size();
    tanks.push_back(new Tank(players[i]));
    players.erase(players.begin() + i);
  }
}

GameRound::~GameRound() {
  for (Tank* tnk: tanks)   delete tnk;
  tanks.clear();
  for (auto pr: particles) delete pr;
  particles.clear();
}

bool GameRound::CreateRound() {
  Map_CreateMap(this);
  InsertTanks();

  refresh = true;

  return true;
}

bool GameRound::InsertTanks() {
  int dist = width / (tanks.size());
  int x = dist / 2, y;
  for (Tank* tnk: tanks) {
    y = 0;
    while (y < height and not Map_IsGround(x, y)) y++;
    for (int xx = x - 5; xx < x + 5; xx++)
      for (int yy = y; yy > 0 and Map_IsGround(xx, yy); yy--)
        Map_ClearPixel(xx, yy);
    tnk->loc = Point(x, y);
    x += dist;
  }

  refresh = true;

  return true;
}

