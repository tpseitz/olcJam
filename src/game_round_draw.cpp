#include "game.h"
#include "physics.h"
#include "draw_pge.h"

std::vector<Area> clean;

DrawRound::DrawRound(olc::PixelGameEngine* eng, GameRound* rnd) {
  pge = eng; round = rnd;
  CreateBackground();
}

DrawRound::~DrawRound() {
  if  (background != NULL) delete background;
}

bool DrawRound::CreateBackground() {
  background = new olc::Sprite(round->width, round->height);

  float mlt = (float)round->height / 255.0;
  int yy, c;
  olc::Pixel col;
  for (float y = 0; y < round->height; y++) {
    c = 255 - (int)(y / mlt);
    if (c < 128) col = olc::Pixel(0, 0, c);
    else col = olc::Pixel((c - 128) / 2, c - 128, 128);
    yy = (int)y;
    for (int x = 0; x < round->width; x++) background->SetPixel(x, yy, col);
  }

  return true;
}

bool DrawRound::ClearArea(Area area) {
  if (background == NULL) pge->FillRect(
    offset.x + area.x, offset.y + area.y, area.w, area.h, olc::BLACK);
  else pge->DrawPartialSprite(offset.x + area.x - 1, offset.y + area.y - 1,
    background, area.x - 1, area.y - 1, area.w + 2, area.h + 2);

  pge->DrawPartialSprite(offset.x + area.x - 1, offset.y + area.y - 1,
    round->ground, area.x - 1, area.y - 1, area.w + 2, area.h + 2);

  return true;
}

bool DrawRound::ClearColumn(int x) {
  if (round->columns[x].s < 0) return true;

  for (int y = round->columns[x].s; y <= round->columns[x].e; y++) {
    olc::Pixel px = round->ground->GetPixel(x, y);
    if (px.a == 0) px = background->GetPixel(x, y);
    pge->Draw(offset.x + x, offset.y + y, px);
  }

  round->columns[x] = Range();

  return true;
}

bool DrawRound::Clear() {
  olc::Pixel::Mode m = pge->GetPixelMode();

  pge->SetPixelMode(olc::Pixel::Mode::MASK);
  for (auto obj: projectiles) ClearArea(obj->GetArea());
  for (auto obj: explosions) ClearArea(obj->GetArea());
  for (Tank* tnk: round->tanks) ClearArea(tnk->GetArea());
  for (int x = 0; x < round->width; x++) ClearColumn(x);
  for (auto par: particles) {
    Point pt = par->loc.GetPoint();
    olc::Pixel px = round->ground->GetPixel(pt.x, pt.y);
    if (px.a == 0) px = background->GetPixel(pt.x, pt.y);
    pge->Draw(offset.x + pt.x, offset.y + pt.y, px);
  }
  for (Area a: clean) ClearArea(a);
  clean.clear();

  return true;
}

bool DrawRound::Draw() {
  olc::Pixel::Mode m = pge->GetPixelMode();
  pge->SetPixelMode(olc::Pixel::Mode::MASK);

  if (round->refresh) {
    pge->FillRect(0, 0, pge->ScreenWidth(), pge->ScreenHeight(), olc::BLACK);

    if (background != NULL) pge->DrawSprite(offset.x, offset.y, background);

    pge->DrawSprite(offset.x, offset.y, round->ground);

    round->refresh = false;
  }

  pge->FillRect(0, 0, pge->ScreenWidth(), 3, { 128, 128, 255 });
  pge->DrawLine(0, 1, pge->ScreenWidth(), 1, olc::BLACK);

  for (Tank* tnk: round->tanks) {
    if (!tnk->alive) continue;
    Point loc = tnk->loc + offset;
    Point ep = loc + VectorFromAngle(tnk->angle, 10).GetPoint();
    pge->DrawLine(loc.x, loc.y, ep.x, ep.y, tnk->player->color);
    pge->FillCircle(loc.x, loc.y, 4, olc::BLACK);
    pge->FillCircle(loc.x, loc.y, 3, tnk->player->color);
  }

  pge->SetPixelMode(m);
  for (auto obj: projectiles) DrawObject(obj);
  for (auto obj: explosions) DrawObject(obj);

  for (auto par: particles) DrawParticle(par);

  if (round->selected_tank != NULL) DrawInterface();

  if (round->state == GameRound::SCORES) DrawScores();

  pge->SetPixelMode(m);

  return true;
}

const uint32_t DOTLINE = 0b10000000100000001000000010000000;
bool DrawRound::DrawInterface() {
  Tank* tank = round->selected_tank;
  char stats[150] = { 0 };
  sprintf(stats,
    "Power: %4d | Angle: %3d | Score: %-10d | Health: %d%% | Wind: %2.1f",
    tank->power, tank->angle, tank->player->score, tank->health, round->wind.x*100.0);
  pge->FillRect(0, 0, pge->ScreenWidth(), 24, olc::BLACK);
  pge->DrawString(4, 4, stats, olc::WHITE, 2);
  if (round->wind.x != 0) {
    int sx = round->width / 2, sy = 32;
    int wx = (int)(round->wind.x * 100.0), ww = (int)(round->wind.x * 10.0);
    if (wx > 0) pge->FillRect(sx, sy - 1, wx, 3, olc::YELLOW);
    else pge->FillRect(sx + wx, sy - 1, -wx, 3, olc::YELLOW);
    pge->FillTriangle(sx+wx, sy-4, sx+wx, sy+4, sx+wx+ww, sy, olc::YELLOW);
  }

  Point loc = tank->loc + offset;
  pge->DrawCircle(loc.x, loc.y, 25, olc::WHITE);
  Point ep = (Vector2D(loc) + VectorFromAngle(tank->angle, tank->power / 5)).GetPoint();
  pge->DrawLine(loc.x, loc.y, ep.x, ep.y, olc::GREY, DOTLINE);
  clean.push_back({ tank->loc, ep - offset });

  for (Tank* tnk: round->tanks) {
    if (!tnk->alive) continue;
    olc::Pixel col = olc::GREEN;
    if      (tnk->health < 10) col = olc::RED;
    else if (tnk->health < 30) col = olc::Pixel(255, 128, 0);
    else if (tnk->health < 50) col = olc::YELLOW;
    int hb = tnk->health * 30 / MAX_HEALTH;
    loc = tnk->loc + offset;
    pge->FillRect(loc.x - 15, loc.y + 10, 30, 2, olc::BLACK);
    pge->FillRect(loc.x - 15, loc.y + 10, hb, 2, col);
  }

  return true;
}

bool DrawRound::DrawScores() {
  char buf[20] = { 0 };
  sprintf(buf, "Round: %3d / %-3d",
    game_rounds - round->rounds_left + 1, game_rounds);
  pge->DrawString(round->width / 2 - 144, 16, buf, olc::WHITE, 2);

  std::sort(players.begin(), players.end(),
    [](const Player* a, const Player* b) { return a->score > b->score; });
  int sx = round->width / 2 - 240, sy = 48, lh = 24, i = 0;
  for (Player* plr: players) {
    sprintf(buf, "%15s   %-10d", plr->name.c_str(), plr->score);
    pge->DrawString(sx, sy + i * lh, buf, olc::WHITE, 2);
    pge->FillRect(sx + 256, sy + i++ * lh + 2, 16, 16, plr->color);
  }

  return true;
}

bool DrawRound::DrawObject(const Object* obj) {
/*
  switch (obj->type) {
    case Object::PROJECTILE: DrawObject(obj->GetProjectile()); break;
    case Object::EXPLOSION:  DrawObject(obj->GetExplosion()); break;
    default:
      Log(0, "Unknown object type");
      return false;
  }
*/
  return true;
}

bool DrawRound::DrawObject(const Projectile* proj) {
  if (!proj->alive) return false;

  Point p = offset + proj->loc.GetPoint();
  if (p.y > 0) pge->FillCircle(p.x, p.y, 1);
  else {
    pge->DrawLine(p.x, 0, p.x, 2);
    pge->DrawLine(p.x - 1, 1, p.x + 1, 1);
  }

  return true;
}

bool DrawRound::DrawObject(const Explosion* exp) {
  if (!exp->alive) return false;

  Point p = offset + exp->loc.GetPoint();
  pge->FillCircle(p.x, p.y, exp->size, exp->color);

  return true;
}

bool DrawRound::DrawParticle(const Particle* par) {
  switch (par->type) {
    case Particle::DEFAULT: {
      Point pt = par->loc.GetPoint() + offset;
      pge->Draw(pt.x, pt.y, par->color);
      break;
    } case Particle::TEXT: {
      Point pt = par->loc.GetPoint() + offset;
      Point sz = par->size;
      Area a = Area(pt.x - 2, pt.y - 2, sz.x + 4, sz.y + 4);
      pge->FillRect(a.x, a.y, a.w, a.h, olc::WHITE);
      pge->DrawString(pt.x, pt.y, par->text, olc::BLACK);
      clean.push_back(a - offset);
      break;
    } default: {
      Log(0, "Unknown particle type for drawing");
    }
  }

  return true;
}

