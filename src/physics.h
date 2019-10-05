#ifndef PHYSICS_H
#define PHYSICS_H

#include "game.h"

extern int PHYSICS_ROUNDS;

struct Particle {
  enum Type { DEFAULT, TEXT };

  // Particle type
  Type type = DEFAULT;
  // Lifetime properties
  float age = 0.0, time = 0.0;
  bool alive = true;
  // Physical properties
  Vector2D loc, speed;
  float gravity = 1.0, wind = 1.0;
  // Particle color
  uint32_t color;

  Particle(const Vector2D, const Vector2D, const float = 1.0,
    const uint32_t = 0xff808080, const float = 10.0, const float = 10.0);

  virtual bool Update(GameRound*, float);

  virtual bool Draw();
};

struct SpeechBalloon : Particle {
  std::string text;
  Point size;

  SpeechBalloon(const Vector2D, const std::string);

  bool Draw() override;
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
  virtual bool Draw();
};

struct Projectile : Object {
  Projectile(Tank*, Vector2D, Vector2D);
  bool Update(GameRound*) override;
  Area GetArea() override;
  bool Draw() override;
};

struct Explosion : Object {
  int size = 100, power = 5, time = 3;
  uint32_t color = 0xff0040ff; //{ 255, 64, 0 };

  Explosion(Tank*, Vector2D, int, int);
  bool Update(GameRound*) override;
  Area GetArea() override;
  bool Draw() override;
};

extern std::vector<Particle*> particles;
extern std::vector<Object*>  objects;

void CleanObjects();

#endif

