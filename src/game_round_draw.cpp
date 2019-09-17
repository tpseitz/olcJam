#include "game.h"
#include "physics.h"
#include "olcPixelGameEngine.h"
#include "util_pge.h"

//TODO Move this to utils and make it universal
olc::Sprite* DiamondSquare(int size, int count) {
  int jump = 16;
  while (jump < size) jump *= 2;
  size = jump * count;

  olc::Sprite* img = new olc::Sprite(size, size);
  for (int x = 0; x < size; x += jump)
    for (int y = 0; y < size; y += jump)
      img->SetPixel(x, y, olc::Pixel(64 + rand() % 128, 0, 0, 255));

  while (jump > 1) {
    jump /= 2;

    for (int x = jump; x < size; x += jump * 2)
      for (int y = jump; y < size; y += jump * 2)
        if (img->GetPixel(x, y).r == 0)
          img->SetPixel(x, y, olc::Pixel((
              img->GetPixel(x - jump, y - jump).r
            + img->GetPixel(x - jump, (y + jump) % size).r
            + img->GetPixel((x + jump) % size, y - jump).r
            + img->GetPixel((x + jump) % size, (y + jump) % size).r
          ) / 4 + rand() % jump - jump / 2, 0, 0, 255));

    for (int x = 0; x < size; x += jump) {
      for (int y = 0; y < size; y += jump) {
        if (img->GetPixel(x, y).r > 0) continue;
        int x1 = x - jump, y1 = y - jump, x2 = x + jump, y2 = y + jump;
        if (x1 < 0) x1 += size;
        if (y1 < 0) y1 += size;
        x2 %= size;
        y2 %= size;
        img->SetPixel(x, y, olc::Pixel((
            img->GetPixel(x1, y).r + img->GetPixel(x2, y).r
          + img->GetPixel(x, y1).r + img->GetPixel(x, y2).r
        ) / 4 + rand() % jump - jump / 2, 0, 0, 255));
      }
    }
  }

  return img;
}

// Variables to hold map and its properties
olc::Sprite* background = NULL;
olc::Sprite* ground = NULL;
Point offset = { 2, 3 };
// Pixel game engine variables
extern olc::PixelGameEngine* pge;
// Variables to hold properties for cleaning map
std::vector<Area> clean;
std::set<int> changed;
std::map<int, Range> columns;

uint8_t Map_IsGround(int x, int y) {
  if (ground == NULL) return 0;
  else return ground->GetPixel(x, y).a;
}

uint32_t Map_GetColor(int x, int y) {
  if (ground == NULL) return 0;
  else return ground->GetPixel(x, y).n;
}

void Map_ClearPixel(int x, int y) {
  if (ground == NULL) return;

  ground->SetPixel(x, y, { 0x00000000 });
}

void Map_ClearCircle(int cx, int cy, int size) {
  if (ground == NULL) return;

  int rr = size * size, yy;
  for (int x = cx - size; x < cx + size; x++)
    if (x >= 0 and x < ground->width)
      changed.insert(x);
  for (int x = 0; x < size; x++) {
    yy = std::sqrt(rr - x * x);
    for (int y = 0; y < yy; y++) {
      if (cx - x >= 0) {
        Map_ClearPixel(cx - x, cy + y);
        Map_ClearPixel(cx - x, cy - y);
      } if (cx + x < ground->width) {
        Map_ClearPixel(cx + x, cy + y);
        Map_ClearPixel(cx + x, cy - y);
      }
    }
  }
}

void Map_MovePixel(int x1, int y1, int x2, int y2) {
  if (ground == NULL) return;

  ground->SetPixel(x2, y2, ground->GetPixel(x1, y1));
  ground->SetPixel(x1, y1, { 0x00000000 });
}

void Map_CreateMap(GameRound* round) {
  // Draw background
  if  (background != NULL) delete background;
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

  // Create profile for ground
  int wdt =
    round->width + round->initial_jump - round->width % round->initial_jump;
  int hght[wdt];
  for (int x = 0; x < wdt; x++) hght[x] = -1;

  int jump = round->initial_jump;
  int range = round->max - round->min;
  for (int x = 0; x < wdt; x += jump) hght[x] = round->min + rand() % range;

  int add = 0;
  while (jump > 1) {
    jump /= 2;
    range /= 2;
    for (int x = jump; x < wdt - jump; x += jump) {
      if (hght[x] < 0) {
        if (range > 1) add = rand() % range - range / 2;
        else add = 0;
        hght[x] = (hght[x-jump] + hght[x+jump]) / 2 + add;
        if (hght[x] < round->min) hght[x] = round->min;
        if (hght[x] > round->max) hght[x] = round->max;
      }
    }
  }

  // Pattern texture for ground
  olc::Sprite* plasma = DiamondSquare(128, 4);
  // Draw the map
  if  (ground != NULL) delete ground;
  ground = new olc::Sprite(round->width, round->height);

  int h = round->height / 2;
  col = olc::Pixel(192, 0, 0);
  for (int x = 0; x < round->width; x++) {
    h = round->height - hght[x];
    if (h < 0) h = 0;
    for (int y = 0; y < h; y++)
      ground->SetPixel(x, y, { 0x00000000 });
    for (int y = h; y < round->height; y++) {
      col = plasma->GetPixel(x % plasma->width, y % plasma->height);
      col.r = (col.r + 255 - y * 255 / round->height) / 2;
      col.g = (64 - y * 64 / round->height) * col.r / 256;
      ground->SetPixel(x, y, col);
    }
  }

  delete plasma;
}

void Map_DestroyMap() {
  if (background != NULL) delete background;
  background = NULL;
  if (ground != NULL) delete ground;
  ground = NULL;
  if (game_round != NULL) delete game_round;
  game_round = NULL;
}

bool ClearArea(Area area) {
  if (background != NULL)
    pge->DrawPartialSprite(offset.x + area.x - 1, offset.y + area.y - 1,
    background, area.x - 1, area.y - 1, area.w + 2, area.h + 2);
  else pge->FillRect(
    offset.x + area.x, offset.y + area.y, area.w, area.h, olc::BLACK);

  pge->DrawPartialSprite(offset.x + area.x - 1, offset.y + area.y - 1,
    ground, area.x - 1, area.y - 1, area.w + 2, area.h + 2);

  return true;
}

bool ClearColumn(int x) {
  if (columns[x].s < 0) return true;

  if (background != NULL) {
    for (int y = columns[x].s; y <= columns[x].e; y++) {
      olc::Pixel px = ground->GetPixel(x, y);
      if (px.a == 0) px = background->GetPixel(x, y);
      pge->Draw(offset.x + x, offset.y + y, px);
    }
  } else {
    for (int y = columns[x].s; y <= columns[x].e; y++) {
      olc::Pixel px = ground->GetPixel(x, y);
      if (px.a == 0) pge->Draw(offset.x + x, offset.y + y, olc::BLACK);
    }
  }

  columns[x] = Range();

  return true;
}

bool DrawRound_Clear() {
  if (ground == NULL) return false;

  olc::Pixel::Mode m = pge->GetPixelMode();

  pge->SetPixelMode(olc::Pixel::Mode::MASK);
  for (auto obj: objects) ClearArea(obj->GetArea());
  for (Tank* tnk: game_round->tanks) ClearArea(tnk->GetArea());
  for (int x = 0; x < game_round->width; x++) ClearColumn(x);
  for (auto par: particles) {
    Point pt = par->loc.GetPoint();
    olc::Pixel px = ground->GetPixel(pt.x, pt.y);
    if (px.a == 0) px = background->GetPixel(pt.x, pt.y);
    pge->Draw(offset.x + pt.x, offset.y + pt.y, px);
  }
  for (Area a: clean) ClearArea(a);
  clean.clear();

  return true;
}

bool Object::Draw() { return false; }

bool Projectile::Draw() {
  if (!alive) return false;

  Point p = offset + loc.GetPoint();
  if (p.y > 0) pge->FillCircle(p.x, p.y, 1);
  else {
    pge->DrawLine(p.x, 0, p.x, 2);
    pge->DrawLine(p.x - 1, 1, p.x + 1, 1);
  }

  return true;
}

bool Explosion::Draw() {
  if (!alive) return true;

  Point p = offset + loc.GetPoint();
  pge->FillCircle(p.x, p.y, size, color);

  return true;
}

bool Particle::Draw() {
  if (!alive) return true;

  Point pt = loc.GetPoint() + offset;
  pge->Draw(pt.x, pt.y, color);

  return true;
}

bool SpeechBalloon::Draw() {
  if (!alive) return true;

  Point pt = loc.GetPoint() + offset;
  Point sz = size;
  Area a = Area(pt.x - 2, pt.y - 2, sz.x + 4, sz.y + 4);
  pge->FillRect(a.x, a.y, a.w, a.h, olc::WHITE);
  pge->DrawString(pt.x, pt.y, text, olc::BLACK);
  clean.push_back(a - offset);

  return true;
}

bool DrawRound_Draw() {
  if (ground == NULL) return false;

  olc::Pixel::Mode m = pge->GetPixelMode();
  pge->SetPixelMode(olc::Pixel::Mode::MASK);

  if (game_round->refresh) {
    pge->FillRect(0, 0, pge->ScreenWidth(), pge->ScreenHeight(), olc::BLACK);

    if (background != NULL) pge->DrawSprite(offset.x, offset.y, background);

    pge->DrawSprite(offset.x, offset.y, ground);

    game_round->refresh = false;
  }

  pge->FillRect(0, 0, pge->ScreenWidth(), 3, { 128, 128, 255 });
  pge->DrawLine(0, 1, pge->ScreenWidth(), 1, olc::BLACK);

  for (Tank* tnk: game_round->tanks) {
    if (!tnk->alive) continue;
    Point loc = tnk->loc + offset;
    Point ep = loc + VectorFromAngle(tnk->angle, 10).GetPoint();
    pge->DrawLine(loc.x, loc.y, ep.x, ep.y, tnk->player->color);
    pge->FillCircle(loc.x, loc.y, 4, olc::BLACK);
    pge->FillCircle(loc.x, loc.y, 3, tnk->player->color);
  }

  pge->SetPixelMode(m);
  for (auto obj: objects) obj->Draw();

  for (auto par: particles) par->Draw();

  if (game_round->selected_tank != NULL)
    DrawRound_Interface(game_round->selected_tank);

  if (game_round->state == GameRound::SCORES) DrawRound_Scores();

  pge->SetPixelMode(m);

  return true;
}

const uint32_t DOTLINE = 0b10000000100000001000000010000000;
bool DrawRound_Interface(Tank* tank) {
  char stats[150] = { 0 };
  sprintf(stats,
    "Power: %4d | Angle: %3d | Score: %-10d | Health: %d%% | Wind: %2.1f",
    tank->power, tank->angle, tank->player->score, tank->health, game_round->wind.x*100.0);
  pge->FillRect(0, 0, pge->ScreenWidth(), 24, olc::BLACK);
  pge->DrawString(4, 4, stats, olc::WHITE, 2);
  if (game_round->wind.x != 0) {
    int sx = game_round->width / 2, sy = 32;
    int wx = (int)(game_round->wind.x * 100.0), ww = (int)(game_round->wind.x * 10.0);
    if (wx > 0) pge->FillRect(sx, sy - 1, wx, 3, olc::YELLOW);
    else pge->FillRect(sx + wx, sy - 1, -wx, 3, olc::YELLOW);
    pge->FillTriangle(sx+wx, sy-4, sx+wx, sy+4, sx+wx+ww, sy, olc::YELLOW);
  }

  Point loc = tank->loc + offset;
  pge->DrawCircle(loc.x, loc.y, 25, olc::WHITE);
  Point ep = (Vector2D(loc) + VectorFromAngle(tank->angle, tank->power / 5)).GetPoint();
  pge->DrawLine(loc.x, loc.y, ep.x, ep.y, olc::GREY, DOTLINE);
  clean.push_back({ tank->loc, ep - offset });

  for (Tank* tnk: game_round->tanks) {
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

bool DrawRound_Scores() {
  char buf[20] = { 0 };
  sprintf(buf, "Round: %3d / %-3d",
    game_rounds - game_round->rounds_left + 1, game_rounds);
  pge->DrawString(game_round->width / 2 - 144, 16, buf, olc::WHITE, 2);

  std::sort(players.begin(), players.end(),
    [](const Player* a, const Player* b) { return a->score > b->score; });
  int sx = game_round->width / 2 - 240, sy = 48, lh = 24, i = 0;
  for (Player* plr: players) {
    sprintf(buf, "%15s   %-10d", plr->name.c_str(), plr->score);
    pge->DrawString(sx, sy + i * lh, buf, olc::WHITE, 2);
    pge->FillRect(sx + 256, sy + i++ * lh + 2, 16, 16, plr->color);
  }

  return true;
}

