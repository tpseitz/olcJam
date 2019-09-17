#include "game.h"

Tank::Tank() {
  player = NULL;
}

Tank::Tank(Player* plr) {
  player = plr;
}

bool Tank::Prepare(const Interface inter) {
  switch (player->type) {
    case Player::HUMAN: {
      angle += inter.angdir;
      power += inter.powdir;
      if (inter.ready) ready = true;

      CheckLimits();

      return true;

    } case Player::RANDOM: {
      angle = rand() % (MAX_ANGLE - MIN_ANGLE) + MIN_ANGLE;
      power = rand() % MAX_POWER;
      ready = true;
      return true;

    } default: {
      ready = true;
      return false;
    }
  }
}

void Tank::CheckLimits() {
  if (angle < MIN_ANGLE) angle = MIN_ANGLE;
  if (angle > MAX_ANGLE) angle = MAX_ANGLE;
  if (power < MIN_POWER) power = MIN_POWER;
  if (power > MAX_POWER) power = MAX_POWER;
}

Area Tank::GetArea() {
  return Area(loc.x - 15, loc.y - 25, 31, 41);
}


