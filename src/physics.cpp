#include "physics.h"

int PHYSICS_ROUNDS = 3;

std::vector<Projectile*> projectiles;
std::vector<Explosion*>  explosions;

void CleanObjects() {
  for (int i = projectiles.size()-1; i >= 0; i--) {
    if (projectiles[i]->alive) continue;
    Projectile* obj = projectiles[i];
    projectiles.erase(projectiles.begin() + i);
    delete obj;
  }
  for (int i = explosions.size()-1; i >= 0; i--) {
    if (explosions[i]->alive) continue;
    Explosion* obj = explosions[i];
    explosions.erase(explosions.begin() + i);
    delete obj;
  }
}

bool Object::Update(GameRound* game) {
  alive = false;
  return false;
}

Object::Object(Tank* parent, Vector2D location) {
  owner = parent;
  loc = location;
}

Area Object::GetArea() {
  return Area((int)loc.x - 3, (int)loc.y - 3, 7, 7);
}

Projectile::Projectile(Tank* parent, Vector2D location, Vector2D speed)
    : Object(parent, location) {
  projectiles.push_back(this);

  this->speed = speed;
  physics = true;
}

bool Projectile::Update(GameRound* game) {
  if (!alive) return false;

  age++;
  if (physics) speed += (game->gravity + game->wind) / 100.0;
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

  if (alive and loc.y > 0 and age % 2 == 0)
    new Particle(loc, {0.0, 0.0}, 100, owner->player->color, -0.05, 0.5);

  return true;
}

Area Projectile::GetArea() {
  return Area((int)loc.x - 2, (int)loc.y - 2, 5, 5);
}

Explosion::Explosion(Tank* parent, Vector2D location, int size, int time)
    : Object(parent, location) {
  explosions.push_back(this);

  this->size = size;
  this->time = time * PHYSICS_ROUNDS;
  physics = false;
}

bool Explosion::Update(GameRound* game) {
  if (!alive) return false;

  Point pl = loc.GetPoint();
  uint32_t col = game->ground->GetPixel(pl.x, pl.y).n;
  if (col & 255 == 0) col = 0x808080ff;
  for (int i = 0; i < power; i++) {
    Vector2D spd = VectorFromAngle(
      rand() % 360, (float)(rand() % 100) / 100.0);
    new Particle(loc + spd, spd, 50 + rand() % 100, col);
  }
  if (age >= time) {
    int rr = size * size, yy;
    olc::Pixel px = olc::Pixel(0, 0, 0, 0);
    for (int x = pl.x - size; x < pl.x + size; x++)
      if (x >= 0 and x < game->width)
        game->changed.insert(x);
    for (int x = 0; x < size; x++) {
      yy = std::sqrt(rr - x * x);
      for (int y = 0; y < yy; y++) {
        if (pl.x - x >= 0) {
          game->ground->SetPixel(pl.x - x, pl.y + y, px);
          game->ground->SetPixel(pl.x - x, pl.y - y, px);
        } if (pl.x + x < game->width) {
          game->ground->SetPixel(pl.x + x, pl.y + y, px);
          game->ground->SetPixel(pl.x + x, pl.y - y, px);
        }
      }
    }
    for (Tank* tnk: game->tanks){
      if (!tnk->alive) continue;
      int dist = (int)(Vector2D(tnk->loc) - loc).length();
      if (dist < size) {
        int dmg = (size - dist) * power;
        tnk->health -= dmg;
        if (tnk == owner) owner->player->score -= dmg / 2;
        else owner->player->score += dmg;
        if (tnk->health < 0) {
          if (tnk == owner) owner->player->score += POINTS_SUICIDE;
          else owner->player->score += POINTS_KILL;
          tnk->alive = false;
          // Create particles
          for (int i = 0; i < 20; i++) {
            Vector2D spd = VectorFromAngle(
              rand() % 360, (float)(rand() % 100) / 100.0);
            new Particle(loc+spd, spd, 50+rand() % 100, tnk->player->color);
          }
          // Display death message
          std::string text = RandomChoice(quotes_death);
          new Particle(Vector2D(tnk->loc) - Vector2D(0.0, 15.0), text);
          Log(5, tnk->player->name + " dies: " + text); 
        }
      }
    }
    alive = false;
  }

  age++;

  return true;
}

Area Explosion::GetArea() {
  return Area((int)loc.x - size, (int)loc.y - size, size * 2 +1, size * 2 +1);
}

