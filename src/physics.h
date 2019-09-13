#ifndef PHYSICS_H
#define PHYSICS_H

#include "game.h"

extern int PHYSICS_ROUNDS;

struct Particle {
  enum Type { DEFAULT, TEXT };

  // Particle type
  Type type = DEFAULT;
  // Lifetime properties
  int age = 0, time = 0;
  bool alive = true;
  // Physical properties
  Vector2D loc, speed;
  float gravity = 1.0, wind = 1.0;
  // Properties for default particle
  uint32_t color;
  // Properties for text
  std::string text;
  Point size;

  Particle(const Vector2D, const Vector2D, const int = 50,
    const uint32_t = 0x808080ff, const float = 0.1, const float = 0.1);

  Particle(const Vector2D, const std::string);

  virtual bool Update(GameRound*);
};

struct Object {
  enum { PROJECTILE, EXPLOSION };

  Tank* owner;
  Vector2D loc = { -1, -1 }, speed = { 0, 0 };
  int age = 0;
  bool physics = true;
  bool alive = true;

  Object(Tank*, Vector2D);
  virtual bool Update(GameRound*);
  virtual Area GetArea();
};

struct Projectile : Object {
  Projectile(Tank*, Vector2D, Vector2D);
  bool Update(GameRound*) override;
  Area GetArea() override;
};

struct Explosion : Object {
  int size = 100, power = 5, time = 3;
  olc::Pixel color = { 255, 64, 0 };

  Explosion(Tank*, Vector2D, int, int);
  bool Update(GameRound*) override;
  Area GetArea() override;
};

extern std::vector<Particle*>   particles;
extern std::vector<Projectile*> projectiles;
extern std::vector<Explosion*>  explosions;

void CleanObjects();

#endif

