#include "physics.h"

std::vector<Object*> objects;

bool Object::Update(GameRound* game) {
  alive = false;
  return false;
}

Object::Object(Tank* parent, Vector2D location) {
  objects.push_back(this);

  owner = parent;
  loc = location;
  if (owner == NULL) std::cout << "No owner!" << std::endl; //XXX
}

Area Object::GetArea() {
  return Area((int)loc.x - 3, (int)loc.y - 3, 7, 7);
}

Projectile::Projectile(Tank* parent, Vector2D location, Vector2D speed)
    : Object(parent, location) {
  this->speed = speed;
  physics = true;
}

bool Projectile::Update(GameRound* game) {
  if (!alive) return false;

  age++;
  if (physics) speed += game->gravity;
  loc += speed;
  Point pl = loc.GetPoint();
  // Check wall collision
  if (pl.x < 0 or pl.x >= game->width) alive = false;
  // Check ground collision
  if (pl.y >= game->height) {
    new Explosion(owner, loc, 30, 10);
    alive = false;
  }
  // Check ground collision
  if (game->ground->GetPixel(pl.x, pl.y).a > 0) {
    new Explosion(owner, loc, 30, 10);
    alive = false;
  }

  return true;
}

Area Projectile::GetArea() {
  return Area((int)loc.x - 2, (int)loc.y - 2, 5, 5);
}

Explosion::Explosion(Tank* parent, Vector2D location, int size, int time)
    : Object(parent, location) {
  this->size = size;
  this->time = time;
  physics = false;
}

bool Explosion::Update(GameRound* game) {
  if (!alive) return false;

  if (age >= time) {
    Point pl = loc.GetPoint();
    int rr = size * size, yy;
    olc::Pixel px = olc::Pixel(0, 0, 0, 0);
    for (int x = 0; x < size; x++) {
      yy = std::sqrt(rr - x * x);
      for (int y = 0; y < yy; y++) {
        game->ground->SetPixel(pl.x + x, pl.y + y, px);
        game->ground->SetPixel(pl.x + x, pl.y - y, px);
        game->ground->SetPixel(pl.x - x, pl.y + y, px);
        game->ground->SetPixel(pl.x - x, pl.y - y, px);
      }
    }
    for (Tank* tnk: game->tanks){
      if (!tnk->alive) continue;
      int dist = (int)(Vector2D(tnk->loc) - loc).length();
      if (dist < size) {
        int dmg = (size - dist) * power;
        tnk->health -= dmg;
        if (tnk == owner) {
          owner->player->score -= dmg / 2;
          if (tnk->health < 0) {
            owner->player->score += POINTS_SUICIDE;
            tnk->alive = false;
          }
        } else {
          owner->player->score += dmg;
          if (tnk->health < 0) {
            owner->player->score += POINTS_KILL;
            tnk->alive = false;
          }
        }
      }
    }
    alive = false;
  }

  return true;
}

Area Explosion::GetArea() {
  return Area((int)loc.x - size, (int)loc.y - size, size * 2 +1, size * 2 +1);
}

