#ifndef DRAW_H
#define DRAW_H

#include "game.h"
#include "physics.h"

struct DrawRound {
  Point offset = { 2, 3 };
  olc::PixelGameEngine* pge;
  GameRound* round;

  DrawRound(olc::PixelGameEngine*, GameRound*);

  ~DrawRound();

  bool CreateMap();

  bool Draw();

  bool Clear();

private:
  bool ClearArea(Area);
  bool ClearColumn(int x);
  bool DrawInterface();
  bool DrawScores();
  bool DrawObject(const Object*);
  bool DrawObject(const Projectile*);
  bool DrawObject(const Explosion*);
  bool DrawParticle(const Particle*);
};

#endif

