#include "physics.h"

int PHYSICS_ROUNDS = 3;

std::vector<Object*> objects;

void CleanObjects() {
  for (int i = objects.size()-1; i >= 0; i--) {
    if (objects[i]->alive) continue;
    Object* obj = objects[i];
    objects.erase(objects.begin() + i);
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
  objects.push_back(this);

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
  if (Map_IsGround(pl.x, pl.y)) {
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
  objects.push_back(this);

  this->size = size;
  this->time = time * PHYSICS_ROUNDS;
  physics = false;
}

bool Explosion::Update(GameRound* game) {
  if (!alive) return false;

  Point pl = loc.GetPoint();
  uint32_t col = Map_GetColor(pl.x, pl.y);
  if (col & 255 == 0) col = 0xff808080;
  for (int i = 0; i < power; i++) {
    Vector2D spd = VectorFromAngle(
      rand() % 360, (float)(rand() % 100) / 100.0);
    new Particle(loc + spd, spd, 50 + rand() % 100, col);
  }
  if (age >= time) {
    Map_ClearCircle(pl.x, pl.y, size);

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
          new SpeechBalloon(Vector2D(tnk->loc) - Vector2D(0.0, 15.0), text);
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

