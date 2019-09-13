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
    or round->ground->GetPixel(pt.x, pt.y).a > 0
    or age > time or age > PARTICLE_MAX_AGE
  ) alive = false;

  return true;
}

Particle::Particle(const Vector2D l, const std::string t) {
  particles.push_back(this);
  int len = t.size();
  type = TEXT;
  text = t;
  size = Point(8 * len, 8);
  loc = l - Point(4 * len, 8);

  speed = { 0.0, -0.05 };
  time = 100 + 20 * len;
  color = 0xffff00ff;
  gravity = 0.0; wind = 0.0;
}

