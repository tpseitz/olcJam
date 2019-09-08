#include "game.h"
#include "physics.h"

bool GameRound::ClearArea(olc::PixelGameEngine* pge, Area area) {
  if (background == NULL) pge->FillRect(
    area.x, area.y, area.w, area.h, olc::BLACK);
  else pge->DrawPartialSprite(
    area.x, area.y, background, area.x, area.y, area.w, area.h);

  pge->DrawPartialSprite(
    area.x, area.y, ground, area.x, area.y, area.w, area.h);

  return true;
}

bool GameRound::ClearRound(olc::PixelGameEngine* pge) {
  olc::Pixel::Mode m = pge->GetPixelMode();

  pge->SetPixelMode(olc::Pixel::Mode::MASK);
  for (auto obj: objects) ClearArea(pge, obj->GetArea());
  for (Tank* tnk: tanks) ClearArea(pge, tnk->GetArea());

  pge->SetPixelMode(m);
  return true;
}

bool GameRound::DrawRound(olc::PixelGameEngine* pge) {
  olc::Pixel::Mode m = pge->GetPixelMode();
  pge->SetPixelMode(olc::Pixel::Mode::MASK);

  if (refresh) {

    pge->FillRect(0, 0, pge->ScreenWidth(), pge->ScreenHeight(), olc::BLACK);

    if (background != NULL) pge->DrawSprite(0, 0, background);

    pge->DrawSprite(0, 0, ground);

    refresh = false;
  }

  for (Tank* tnk: tanks) {
    if (!tnk->alive) continue;
    Point ep = tnk->loc + VectorFromAngle(tnk->angle, 10).GetPoint();
    pge->DrawLine(tnk->loc.x, tnk->loc.y, ep.x, ep.y, tnk->player->color);
    pge->FillCircle(tnk->loc.x, tnk->loc.y, 4, olc::BLACK);
    pge->FillCircle(tnk->loc.x, tnk->loc.y, 3, tnk->player->color);
  }

  for (auto obj: objects) obj->Draw(pge);

  if (selected_tank != NULL) DrawInterface(pge, selected_tank);

  if (state == SCORES) DrawScores(pge);

  pge->SetPixelMode(m);

  return true;
}

bool GameRound::DrawInterface(olc::PixelGameEngine* pge, Tank* tank) {
  char stats[46] = { 0 };
  sprintf(stats, "Power: %4d | Angle: %3d | Score: %-10d",
    tank->power, tank->angle, tank->player->score);
  pge->FillRect(0, 0, pge->ScreenWidth(), 24, olc::BLACK);
  pge->DrawString(4, 4, stats, olc::WHITE, 2);
  pge->DrawCircle(tank->loc.x, tank->loc.y, 25, olc::WHITE);

  for (Tank* tnk: tanks) {
    if (!tnk->alive) continue;
    olc::Pixel col = olc::GREEN;
    if      (tnk->health < 10) col = olc::RED;
    else if (tnk->health < 30) col = olc::Pixel(255, 128, 0);
    else if (tnk->health < 50) col = olc::YELLOW;
    int hb = tnk->health * 30 / MAX_HEALTH;
    pge->FillRect(tnk->loc.x - 15, tnk->loc.y - 17, 30, 2, olc::BLACK);
    pge->FillRect(tnk->loc.x - 15, tnk->loc.y - 17, hb, 2, col);
//    pge->DrawString(tnk->loc.x - 10, tnk->loc.y - 23, std::to_string(tnk->health)); //XXX
  }

  return true;
}

bool GameRound::DrawScores(olc::PixelGameEngine* pge) {
  char buf[20] = { 0 };
  sprintf(buf, "Round: %3d / %-3d", game_rounds - rounds_left + 1, game_rounds);
  pge->DrawString(width / 2 - 144, 16, buf, olc::WHITE, 2);

  std::sort(players.begin(), players.end(),
    [](const Player* a, const Player* b) { return a->score > b->score; });
  int sx = width / 2 - 240, sy = 48, lh = 24, i = 0;
  for (Player* plr: players) {
    sprintf(buf, "%15s   %-10d", plr->name.c_str(), plr->score);
    pge->DrawString(sx, sy + i * lh, buf, olc::WHITE, 2);
    pge->FillRect(sx + 256, sy + i++ * lh + 2, 16, 16, plr->color);
  }

  return true;
}

bool Object::Draw(olc::PixelGameEngine* pge) { return true; }

bool Projectile::Draw(olc::PixelGameEngine* pge) {
  if (!alive) return false;

  Point p = loc.GetPoint();
  pge->FillCircle(p.x, p.y, 1);

  return true;
}

bool Explosion::Draw(olc::PixelGameEngine* pge) {
  if (!alive) return false;

  age++;

  Point p = loc.GetPoint();
  pge->FillCircle(p.x, p.y, size, color);

  return true;
}

