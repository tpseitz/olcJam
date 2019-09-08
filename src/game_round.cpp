#include "game.h"

std::vector<Player*> players;
int game_rounds = 5;

Player::Player(std::string name, olc::Pixel col, PlayerType tp) {
  score = 0;
  this->name = name;
  color = col;
  type = tp;
}

void Player::SwitchType() {
  switch (type) {
    case HUMAN: type = RANDOM; break;
    case RANDOM: type = HUMAN; break;
  }
}

Area::Area(int lx, int ly, int width, int height) {
  x = lx; y = ly; w = width; h = height;
}

GameRound::GameRound(int w, int h, std::vector<Player*> players, int rounds) {
  width = w;
  height = h;
  rounds_left = rounds;
  max = height - 20;

  int ndx = 0;
  while (players.size() > 0) {
    int i = rand() % players.size();
    tanks.push_back(new Tank(players[i]));
    players.erase(players.begin() + i);
  }
}

GameRound::~GameRound() {
  for (Tank* tnk: tanks) delete tnk;
  if (background != NULL) delete background;
  if (ground != NULL) delete ground;
}

bool GameRound::CreateRound() {
  CreateMap();
  InsertTanks();

  refresh = true;

  return true;
}

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

  olc::Sprite* plasma = DiamondSquare(128, 4);

  int h = height / 2;
  col = olc::Pixel(192, 0, 0);
  for (int x = 0; x < width; x++) {
    h = height - hght[x];
    if (h < 0) h = 0;
    for (int y = 0; y < h; y++)
      ground->SetPixel(x, y, { 0x00000000 });
    for (int y = h; y < height; y++) {
      col = plasma->GetPixel(x % plasma->width, y % plasma->height);
      col.r = (col.r + 255 - y * 255 / height) / 2;
//      col.b = 255 - y * 256 / height;
      ground->SetPixel(x, y, col);
    }
  }

  delete plasma;

  return true;
}

bool GameRound::InsertTanks() {
  int dist = width / (tanks.size());
  int x = dist / 2, y;
  for (Tank* tnk: tanks) {
    for (y = 0; y < height; y++)
      if (ground->GetPixel(x, y).a > 0) break;
    for (int xx = x - 5; xx < x + 5; xx++)
      for (int yy = y; yy > 0 and ground->GetPixel(xx, yy).a > 0; yy--)
        ground->SetPixel(xx, yy, { 0x00000000 });
    tnk->loc = Point(x, y);
    x += dist;
  }

  refresh = true;

  return true;
}

