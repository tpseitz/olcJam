#include "olcPixelGameEngine.h"
#include "game.h"
#include "draw_pge.h"
#include "menu.h"

const float KEY_TIME = 0.1;
int WIDTH = 1200, HEIGHT = 900, PIXEL_SIZE = 1;

class DestructionClient : public olc::PixelGameEngine {
  bool running = false, paused = false;
  GameRound* round = NULL;
  DrawRound* round_draw = NULL;
  MainMenu* menu = NULL;
  int rounds_left = 0;
  float fTime = 0.0;
  // Keyboard timing
  float fNextKeyTime = 0.0;

  void MainLoop();

public:
  DestructionClient() {
    sAppName = "Destructed earth";
  }

  bool OnUserCreate() override {
    running = true;

    running &= Init();

    srand(time(NULL));

    menu = new MainMenu();

    return running;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    fTime += fElapsedTime;
    if (GetKey(olc::Key::ESCAPE).bPressed) {
      if (round != NULL) round->refresh = true;
      paused = !paused;
    }
    if (paused && GetKey(olc::Key::F10).bPressed) running = false;

    if (paused) {
      int tx = WIDTH / 2 - 240, ty = HEIGHT / 2 - 16;
      FillRect(tx - 8, ty - 8, 560, 48, olc::BLACK);
      DrawString(tx, ty, "Press F10 to exit", { 0xffffffff }, 4);
    } else MainLoop();

    return running;
  }

  bool OnUserDestroy() override {
    if (round != NULL) delete round;
    if (menu != NULL) delete menu;

    for (Player* pp: players) delete pp;

    return true;
  }

  Interface ReadKeys(olc::PixelGameEngine* pge) {
    Interface inter;
    if (pge->GetKey(olc::Key::SPACE).bPressed) inter.ready = true;

    if (fTime < fNextKeyTime) return inter;

    int spd = 1;
    if (pge->GetKey(olc::Key::CTRL).bHeld)  spd *= 10;
    if (pge->GetKey(olc::Key::SHIFT).bHeld) spd *= 100;

    if (pge->GetKey(olc::Key::RIGHT).bHeld) {
      inter.angdir =  spd;
      fNextKeyTime = fTime + KEY_TIME;
    } else if (pge->GetKey(olc::Key::LEFT).bHeld) {
      inter.angdir = -spd;
      fNextKeyTime = fTime + KEY_TIME;
    } if (pge->GetKey(olc::Key::UP).bHeld) {
      inter.powdir =  spd;
      fNextKeyTime = fTime + KEY_TIME;
    } else if (pge->GetKey(olc::Key::DOWN).bHeld) {
      inter.powdir = -spd;
      fNextKeyTime = fTime + KEY_TIME;
    }

    return inter;
  }
};

void DestructionClient::MainLoop() {
  if (menu != NULL) {
    menu->Update(this);
    menu->DrawMenu(this);
    rounds_left = game_rounds;
    if (menu->quit) running = false;
    if (menu->ready) {
      delete menu;
      menu = NULL;
    }
  } else if (round != NULL) {
    if (round_draw == NULL) round_draw = new DrawRound(this, round);
    round_draw->Clear();
    CleanObjects();
    round->Update(ReadKeys(this));
    round_draw->Draw();
    if (round->state == GameRound::END) {
      rounds_left--;
      delete round_draw;
      delete round;
      round_draw = NULL;
      round = NULL;
    }
  } else if (rounds_left > 0) {
    round = new GameRound(WIDTH - 4, HEIGHT - 5, players, rounds_left);
    round->CreateRound();
  } else {
    menu = new MainMenu();
  }
}

int main(int args, char** argv) {
  DestructionClient cli;
  if (cli.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) cli.Start();
  return 0;
}

