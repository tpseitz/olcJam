#include "physics.h"

int PHYSICS_ROUNDS = 3;

bool GameRound::Update(olc::PixelGameEngine* pge) {
  switch (state) {
    case PREPARATION: return UpdatePreparation(pge);
    case SHOOT: return UpdateShoot();
    case SHOOTING: return UpdateShooting();
    case ENDING: return UpdateEnding();
    case END: return true;
    default: return false;
  }
}

bool GameRound::UpdatePreparation(olc::PixelGameEngine* pge) {
  // Loop thru all tanks and call prepare for first alive tank that is not
  // ready
  Tank* tank;
  for (int i = 0; i < tank_count; i++) {
    tank = &tanks[i];
    if (tank->alive && !tank->ready) {
      tank->Prepare(pge);
      return true;
    }
  }

  // If all tanks are ready. Switch to shoot state
  state = SHOOT;
  return true;
}

bool GameRound::UpdateShoot() {
  for (auto obj: objects) delete obj;
  objects.clear();
  tic = 0;

  Tank* tank;
  for (int i = 0; i < tank_count; i++) {
    tank = &tanks[i];
    if (tank->alive) {
      Vector2D shot = VectorFromAngle(tank->angle, (float)tank->power / 100.0);
      Vector2D loc = Vector2D(tank->loc) + shot;
      new Projectile(tank, loc, shot);
    }
  }

  refresh = true;
  state = SHOOTING;
  return true;
}

bool GameRound::UpdateShooting() {
  for (int i = 0; i < PHYSICS_ROUNDS; i++) {
    int alive = 0;
    for (auto obj: objects) {
      if (!obj->alive) continue;
      alive++;
      obj->Update(this);
    }

    for (Tank* tnk = &tanks[0]; tnk < &tanks[tank_count]; tnk++) {
      if (!tnk->alive) continue;
      if (ground->GetPixel(tnk->loc.x, tnk->loc.y).a == 0
          and tnk->loc.y < height) {
        alive++;
        tnk->fall++;
        tnk->loc.y++;
      } else if (tnk->fall > 0) {
        tnk->health -= tnk->fall / 10;
        tnk->fall = 0;
      }
      if (tnk->health <= 0) tnk->alive = false;
    }

    tic++;

    if (alive == 0) {
      state = ENDING;
      break;
    }
  }

  return true;
}

bool GameRound::UpdateEnding() {
  int alive = 0;
  for (int i = 0; i < tank_count; i++) {
    tanks[i].ready = false;
    if (tanks[i].alive) alive++;
  }

  if (alive <= 1) state = END;
  else state = PREPARATION;

  refresh = true;
  return true;
}

