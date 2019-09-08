#ifndef PHYSICS_H
#define PHYSICS_H

#include "game.h"

struct Particle {
  enum Type { DEFAULT, SMOKE };
  olc::Sprite* face = NULL;
  olc::Pixel color = { 255, 255, 255 };
  Vector2D loc, speed;
  Type type = DEFAULT;
};

struct Object {
  Tank* owner;
  Vector2D loc = { -1, -1 }, speed = { 0, 0 };
  int age = 0;
  bool physics = true;
  bool alive = true;

  Object(Tank*, Vector2D);
  virtual bool Update(GameRound*);
  virtual bool Draw(olc::PixelGameEngine*);
  virtual Area GetArea();
};

struct Projectile : Object {
  Projectile(Tank*, Vector2D, Vector2D);
  bool Update(GameRound*) override;
  bool Draw(olc::PixelGameEngine*) override;
  Area GetArea() override;
};

struct Explosion : Object {
  int size = 100, power = 5, time = 3;
  olc::Pixel color = { 255, 64, 0 };

  Explosion(Tank*, Vector2D, int, int);
  bool Update(GameRound*) override;
  bool Draw(olc::PixelGameEngine*) override;
  Area GetArea() override;
};

extern std::vector<Object*> objects;

#endif
