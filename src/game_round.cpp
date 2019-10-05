#include "game.h"
#include "physics.h"

// Active configuration
Configuration configuration;
// Variable for game round
GameRound* game_round = NULL;
// Quote lists
std::vector<std::string> npc_names;
std::vector<std::string> quotes_shoot;
std::vector<std::string> quotes_death;

int game_rounds = 5;

Player::Player(std::string name, uint32_t col, Player::Type tp) {
  score = 0;
  this->name = name;
  color = col;
  type = tp;
}

void Player::SwitchType() {
  switch (type) {
    case Player::DUMMY:  type = Player::RANDOM; break;
    case Player::RANDOM: type = Player::HUMAN; break;
    case Player::HUMAN:  type = Player::DUMMY; break;
  }
}

Configuration::Configuration() {}

Configuration::~Configuration() {
  for (Player* pp: players) delete pp;
}

void Configuration::Init() {
  for (int i = 0; i < player_count; i++)
    players.push_back(new Player("Player " + std::to_string(i),
      COLOR_LIST[i % COLOR_COUNT], configuration.default_player_type));
}

bool Game_Init() {
  npc_names    = ReadLines("data/names.txt");
  quotes_shoot = ReadLines("data/shoot.txt");
  quotes_death = ReadLines("data/death.txt");

  configuration.Init();

  return true;
}

bool Game_Prepare() {
  std::set<int> names_exclude;
  int di = 0;
  for (Player* plr: configuration.players) {
    // Select random name for computer players
    if (plr->type == Player::RANDOM)
      plr->name = RandomChoice(npc_names, &names_exclude);
    // All dummy tanks are called Bob
    if (plr->type == Player::DUMMY)
      plr->name = "Bob " + std::to_string(di++);
    // Reset score for each player
    plr->score = 0;
  }

  return true;
}

GameRound::GameRound(const Configuration* cfg, const int rnd) {
  if (game_round != NULL) delete game_round;
  game_round = this;

  config = cfg;
  round = rnd;
  width = cfg->width;
  height = cfg->height;
  wall = cfg->wall;
  max = height - 20;
  gravity = cfg->gravity;

  if (cfg->wind)
    wind = { (float)((double)((rand() % 1000) - 500) / 1000.0), 0.0 };
  else wind = { 0.0, 0.0 };

  if (wall == Configuration::RANDOM) {
    switch (rand() % 4) {
      case 0: wall = Configuration::NONE; break;
      case 1: wall = Configuration::CONCRETE; break;
      case 2: wall = Configuration::RUBBER; break;
      case 3: wall = Configuration::WARP; break;
    }
  }

  std::vector<Player*> tmp = cfg->players;
  while (tmp.size() > 0) {
    int i = rand() % tmp.size();
    tanks.push_back(new Tank(tmp[i]));
    tmp.erase(tmp.begin() + i);
  }

  Log(5, "Round " + std::to_string(round) + " of " \
    + std::to_string(config->rounds));
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

