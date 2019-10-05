#ifndef PLAYERS_H
#define PLAYERS_H

// Constant limits
const int MIN_ANGLE = -95, MAX_ANGLE =  95;
const int MIN_POWER = 10, MAX_POWER = 1500;
const int MAX_HEALTH = 100; //TODO Make this depend on tank type

struct Player {
  enum Type { DUMMY, HUMAN, RANDOM };

  Type type = DUMMY;
  std::string name = "[no name]";
  uint32_t color = 0xffff00ff;
  int64_t score = 0;

  Player(std::string, uint32_t, Type);
  void SwitchType();
};

struct Tank {
  Player* player = NULL;
  Point loc = { -1, -1 };
  int angle = 0, power = 200;
  int health = MAX_HEALTH, fall = 0;
  bool alive = true;
  bool ready = false;

  Tank();
  Tank(Player*);

  bool Prepare();
  bool Update();
  void CheckLimits();
  Area GetArea();
};

#endif
