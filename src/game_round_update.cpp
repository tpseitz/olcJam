#include "physics.h"

int PHYSICS_ROUNDS = 3;

bool GameRound::Update(const Interface inter) {
  if (state != prev_state) tic = 0;
  prev_state = state;

  switch (state) {
    case PREPARATION: return UpdatePreparation(inter);
    case SHOOT: return UpdateShoot();
    case SHOOTING: return UpdateShooting();
    case ENDING: return UpdateEnding();
    case SCORES: return UpdateScores(inter);
    case END: return true;
    default: return false;
  }
}

bool GameRound::UpdatePreparation(const Interface inter) {
  // Loop thru all tanks and call prepare for first alive tank that is not
  // ready
  Tank* tank;
  for (Tank* tnk: tanks) {
    if (tnk->alive && !tnk->ready) {
      if (selected_tank != tnk) refresh = true;
      selected_tank = tnk;
      tnk->Prepare(inter);
      return true;
    }
  }

  // If all tanks are ready. Switch to shoot state
  selected_tank = NULL;
  state = SHOOT;
  return true;
}

bool GameRound::UpdateShoot() {
  for (auto obj: objects) delete obj;
  objects.clear();
  tic = 0;

  for (Tank* tnk: tanks) {
    if (tnk->alive) {
      Vector2D shot = VectorFromAngle(tnk->angle, (float)tnk->power / 100.0);
      Vector2D loc = Vector2D(tnk->loc) + shot;
      new Projectile(tnk, loc, shot);
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

    for (Tank* tnk: tanks) {
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
  for (auto obj: objects) delete obj;
  objects.clear();

  int alive = 0;
  for (Tank* tnk: tanks) {
    tnk->ready = false;
    if (tnk->alive) alive++;
  }

  if (alive <= 1) {
    for (Tank* tnk: tanks) if (tnk->alive) tnk->player->score += POINTS_WINNER;
    state = SCORES;
  } else state = PREPARATION;

  refresh = true;
  return true;
}

bool GameRound::UpdateScores(const Interface inter) {
  int humans = 0;
  for (Tank* tnk: tanks)
    if (tnk->player->type == HUMAN) humans++;

  if (tic > 5 and inter.ready) state = END;
  else if (tic > 300 and humans == 0) state = END;

  tic++;
  return true;
}

