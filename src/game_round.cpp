#include "game.h"
#include "physics.h"

Player::Player(olc::Pixel col, PlayerType tp) {
  color = col;
  type = tp;
}

Area::Area(int lx, int ly, int width, int height) {
  x = lx; y = ly; w = width; h = height;
}

Tank::Tank() {
  player = NULL;
}

Tank::Tank(Player* plr) {
  player = plr;
}

bool Tank::Prepare(olc::PixelGameEngine* pge) {
  switch (player->type) {
    case HUMAN: {
      int spd = 1;
      if (pge->GetKey(olc::Key::SHIFT).bHeld) spd *= 10;
      if (pge->GetKey(olc::Key::CTRL).bHeld)  spd *= 100;

      if (pge->GetKey(olc::Key::RIGHT).bHeld)  angle += spd;
      if (pge->GetKey(olc::Key::LEFT).bHeld) angle -= spd;
      if (pge->GetKey(olc::Key::UP).bHeld)    power += spd;
      if (pge->GetKey(olc::Key::DOWN).bHeld)  power -= spd;

      if (pge->GetKey(olc::Key::SPACE).bPressed) ready = true;

      if (angle < MIN_ANGLE) angle = MIN_ANGLE;
      if (angle > MAX_ANGLE) angle = MAX_ANGLE;
      if (power < 0) power = 0;
      if (power > MAX_POWER) power = MAX_POWER;

      char stats[50];
      sprintf(stats, "Power: %3d | Angle: %3d", power, angle);
      pge->FillRect(0, 0, 388, 20, olc::BLACK);
      pge->DrawString(2, 2, stats, olc::WHITE, 2);

      return true;

    } case RANDOM: {
      angle = rand() % (MAX_ANGLE - MIN_ANGLE) + MIN_ANGLE;
      power = rand() % MAX_POWER;
      power = MAX_POWER; //XXX
      ready = true;
      return true;

    } default: {
      alive = false;
      ready = true;
      return false;
    }
  }
}

Area Tank::GetArea() {
  return Area(loc.x - 15, loc.y - 25, 31, 41);
}

GameRound::GameRound(int w, int h, std::vector<Player*> players) {
  width = w;
  height = h;
  max = height - 20;

  tank_count = 0;
  int ndx = 0;
  while (players.size() > 0) {
    int i = rand() % players.size();
    tanks[ndx++] = players[i];
    players.erase(players.begin() + i);
    tank_count++;
  }
}

GameRound::~GameRound() {
  if (ground != NULL) delete ground;
}

bool GameRound::CreateRound() {
  CreateMap();
  InsertTanks();

  refresh = true;

  return true;
}

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
  for (int i = 0; i < tank_count; i++) ClearArea(pge, tanks[i].GetArea());

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

//  pge->SetPixelMode(olc::Pixel::Mode::NORMAL);

  for (Tank* tnk = &tanks[0]; tnk < &tanks[tank_count]; tnk++) {
    if (!tnk->alive) continue;
    Point ep = tnk->loc + VectorFromAngle(tnk->angle, 10).GetPoint();
    pge->DrawLine(tnk->loc.x, tnk->loc.y, ep.x, ep.y, tnk->player->color);
    pge->FillCircle(tnk->loc.x, tnk->loc.y, 3, tnk->player->color);
    pge->DrawString(tnk->loc.x - 10, tnk->loc.y - 23, std::to_string(tnk->health)); //XXX
  }

  for (auto obj: objects) obj->Draw(pge);

  pge->SetPixelMode(m);

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

bool GameRound::CreateMap() {
  background = new olc::Sprite(width, height);

  float mlt = (float)height / 255.0;
  int yy, c;
  olc::Pixel col;
  for (float y = 0; y < height; y++) {
    c = 255 - (int)(y / mlt);
    if (c < 128) col = olc::Pixel(0, 0, c);
    else col = olc::Pixel((c - 128) / 2, c - 128, 128);
    yy = (int)y;
    for (int x = 0; x < width; x++) background->SetPixel(x, yy, col);
  }

  ground = new olc::Sprite(width, height);

  int wdt = width + initial_jump - width % initial_jump;
  int hght[wdt];
  for (int x = 0; x < wdt; x++) hght[x] = -1;

  int jump = initial_jump;
  int range = max - min;
  for (int x = 0; x < wdt; x += jump)
    hght[x] = min + rand() % range;

  int add = 0;
  while (jump > 1) {
    jump /= 2;
    range /= 2;
    for (int x = jump; x < wdt - jump; x += jump) {
      if (hght[x] < 0) {
        if (range > 1) add = rand() % range - range / 2;
        else add = 0;
        hght[x] = (hght[x-jump] + hght[x+jump]) / 2 + add;
      }
    }
  }

  int h = height / 2;
  col = olc::Pixel(192, 0, 0);
  for (int x = 0; x < width; x++) {
    h = height - hght[x];
    if (h < 0) h = 0;
    for (int y = 0; y < h; y++)
      ground->SetPixel(x, y, { 0x00000000 });
    for (int y = h; y < height; y++)
      ground->SetPixel(x, y, col);
  }

  return true;
}

bool GameRound::InsertTanks() {
  int dist = width / (tank_count), y;
  for (int i = 0, x = dist / 2; i < tank_count; i++) {
    for (y = 0; y < height; y++)
      if (ground->GetPixel(x, y).a > 0) break;
    for (int xx = x - 5; xx < x + 5; xx++)
      for (int yy = y; yy > 0 and ground->GetPixel(xx, yy).a > 0; yy--)
        ground->SetPixel(xx, yy, { 0x00000000 });
    tanks[i].loc = Point(x, y);
    tanks[i].angle = rand() % 180 - 90; //XXX
    x += dist;
  }

  refresh = true;

  return true;
}

