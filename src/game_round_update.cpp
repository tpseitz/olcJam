#include <chrono>
#include "physics.h"

const uint32_t FIRE_COLORS[] = { 0xff0000ff, 0xff0080ff, 0xff00ffff };

// Variables to calculate particle timing
auto t_part = std::chrono::system_clock::now();
auto t_tmp = std::chrono::system_clock::now();

bool GameRound::UpdateParticles() {
  t_tmp = std::chrono::system_clock::now();
  std::chrono::duration<float> dur = t_tmp - t_part;
  t_part = t_tmp;

  for (auto par: particles) par->Update(this, dur.count());

  for (Tank* tnk: tanks) {
    if (not tnk->alive) continue;
    if (tnk->health < 7) {
      new Particle(tnk->loc, VectorFromAngle(rand() % 360, 2.5),
        (float)(50 + rand() % 100) / 100.0, FIRE_COLORS[rand() % 3], -5.0, 1.0);
    if (rand() % 3 == 0)
        new Particle(tnk->loc, VectorFromAngle(rand() % 360, 5.0),
          (float)(50 + rand() % 200) / 100.0, 0xff808080, -5.0, 20.0);
    } else if (tnk->health < 25 and rand() % (tnk->health/5) == 0)
      new Particle(tnk->loc, VectorFromAngle(rand() % 360, 5.0),
        (float)(50 + rand() % 200) / 100.0, 0xff808080, -5.0, 20.0);
  }

  for (int i = particles.size() - 1; i >= 0; i--) if (!particles[i]->alive) {
    Particle* par = particles[i];
    particles.erase(particles.begin() + i);
    delete par;
  }

  return true;
}

bool GameRound::Update() {
  if (state != prev_state) tic = 0;
  prev_state = state;

  switch (state) {
    case PREPARATION: return UpdatePreparation();
    case SHOOT: return UpdateShoot();
    case SHOOTING: return UpdateShooting();
    case ENDING: return UpdateEnding();
    case SCORES: return UpdateScores();
    case END: return true;
    default: return false;
  }
}

bool GameRound::UpdatePreparation() {
  for (int i = 0; i < PHYSICS_ROUNDS; i++) UpdateParticles();

  // Loop thru all tanks and call prepare for first alive tank that is not
  // ready
  Tank* tank;
  for (Tank* tnk: tanks) {
    if (tnk->alive && !tnk->ready) {
//      if (selected_tank != tnk) refresh = true;
      selected_tank = tnk;
      tnk->Prepare();
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
    if (tnk->alive and tnk->player->type != Player::DUMMY) {
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

  int alive = 0, dummies = 0;
  for (Tank* tnk: tanks) {
    tnk->ready = false;
    if (tnk->alive)
      if (tnk->player->type != Player::DUMMY) alive++;
      else dummies++;
  }

  if (alive > 0 and dummies > 0) state = PREPARATION;
  else if (alive <= 1) {
    Player* winner = NULL;
    for (Tank* tnk: tanks) {
      if (tnk->alive) {
        tnk->player->score += config->points_winning;
        winner = tnk->player;
      }
    }
    if (winner == NULL) Log(5, "Round ended. No winner");
    else Log(5, "Round ended. Winner is " + winner->name);
    state = SCORES;
  } else state = PREPARATION;

  refresh = true;
  return true;
}

bool GameRound::UpdateScores() {
  for (int i = 0; i < PHYSICS_ROUNDS; i++) UpdateParticles();

  int humans = 0;
  for (Tank* tnk: tanks)
    if (tnk->player->type == Player::HUMAN) humans++;

//  if (tic > 25 and inter.ready) { state = END; refresh = true; }
  if (tic > 300 and humans == 0) { state = END; refresh = true; }

  tic++;
  return true;
}

