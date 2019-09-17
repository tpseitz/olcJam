#include "physics.h"

const uint32_t FIRE_COLORS[] = { 0xff0000ff, 0xff0080ff, 0xff00ffff };

bool GameRound::UpdateParticles() {
  for (auto par: particles) par->Update(this);

  for (Tank* tnk: tanks) {
    if (not tnk->alive) continue;
    if (tnk->health < 7) {
      new Particle(tnk->loc, VectorFromAngle(rand() % 360, 0.025),
        50 + rand() % 100, FIRE_COLORS[rand() % 3], -0.05, 0.01);
    if (rand() % 3 == 0)
        new Particle(tnk->loc, VectorFromAngle(rand() % 360, 0.05),
          10 + rand() % 200, 0xff808080, -0.05, 0.01);
    } else if (tnk->health < 25 and rand() % (tnk->health/5) == 0)
      new Particle(tnk->loc, VectorFromAngle(rand() % 360, 0.05),
        50 + rand() % 100, 0xff808080, -0.05, 0.2);
  }

  for (int i = particles.size() - 1; i >= 0; i--) if (!particles[i]->alive) {
    Particle* par = particles[i];
    particles.erase(particles.begin() + i);
    delete par;
  }

  return true;
}

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
  for (int i = 0; i < PHYSICS_ROUNDS; i++) UpdateParticles();

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
      if (rand() % 10 == 0) {
        std::string text = RandomChoice(quotes_shoot);
        new SpeechBalloon(loc - Vector2D(0.0, 15.0), text);
        Log(5, tnk->player->name + " shoots: " + text);
      }
    }
  }

  refresh = true;
  state = SHOOTING;
  return true;
}

bool GameRound::UpdateShooting() {
  for (int i = 0; i < PHYSICS_ROUNDS; i++) {
    UpdateParticles();

    int alive = 0;

    std::set<int> del;
    for (int x: changed) {
      if (columns[x].s < 0) {
        int sy = 0, ey = height - 1;
        while (ey > 0 and Map_IsGround(x, ey)) ey--;
        while (sy < height and not Map_IsGround(x, sy)) sy++;
        if (sy + 1 >= ey) {
          del.insert(x);
          continue;
        }
        columns[x] = Range(sy, ey);
      }

      alive++;
      int minY = 0, maxY = 0;
      bool changes = false, drop = false;
      for (int y = columns[x].e; y >= columns[x].s; y--) {
        if (Map_IsGround(x, y)) {
          minY = y;
          if (drop) {
            Map_MovePixel(x, y, x, y + 1);
            changes = true;
          } else {
            drop = false;
          }
        } else {
          maxY = std::max(maxY, y + 1);
          drop = true;
        }
      }
      columns[x].Update(minY, maxY);
    }
    for (int e: del) changed.erase(e);

    for (auto obj: objects) {
      if (!obj->alive) continue;
      alive++;
      obj->Update(this);
    }

    for (Tank* tnk: tanks) {
      if (!tnk->alive) continue;
      if (tnk->loc.y < height and not Map_IsGround(tnk->loc.x, tnk->loc.y)) {
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

  wind.x += (float)((double)((rand() % 100) - 50) / 1000.0);

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
  for (int i = 0; i < PHYSICS_ROUNDS; i++) UpdateParticles();

  int humans = 0;
  for (Tank* tnk: tanks)
    if (tnk->player->type == Player::HUMAN) humans++;

  if (tic > 5 and inter.ready) { state = END; refresh = true; }
  else if (tic > 300 and humans == 0) { state = END; refresh = true; }

  tic++;
  return true;
}

