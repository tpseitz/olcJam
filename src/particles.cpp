#include "physics.h"
#include "util.h"

const int PARTICLE_MAX_AGE = 500;

std::vector<Particle*> particles;

Particle::Particle(const Vector2D l, const Vector2D s, const int t,
    const uint32_t c, const float g, const float w) {
  particles.push_back(this);

  loc = l; speed = s;
  time = t;
  color = c;
  gravity = g; wind = w;
}

bool Particle::Update(GameRound* round) {
  speed += (round->gravity * gravity + round->wind * wind) / 100.0;
  loc += speed;
  age++;
  Point pt = loc.GetPoint();
  if (loc.x < 0 or loc.x > round->width or loc.y < 0 or loc.y > round->height
    or Map_IsGround(pt.x, pt.y) or age > time or age > PARTICLE_MAX_AGE
  ) alive = false;

  return true;
}

SpeechBalloon::SpeechBalloon(const Vector2D l, const std::string t)
    : Particle(l, { 0.0, -0.05 }, 200, 0xffff00ff, 0.0, 0.0) {
  int len = t.size();
  type = TEXT;
  text = t;
  size = Point(8 * len, 8);
  loc = l - Point(4 * len, 8);
  time = 100 + 20 * len;
}

