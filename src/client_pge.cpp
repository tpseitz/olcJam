#include "game.h"
#include "physics.h"
#include "olcPixelGameEngine.h"
#include "menu.h"

const float KEY_TIME = 0.1;
olc::PixelGameEngine* pge = NULL;
int WIDTH = 1200, HEIGHT = 900, PIXEL_SIZE = 1;

class DestructionClient : public olc::PixelGameEngine {
  bool running = false, paused = false;
  MainMenu* menu = NULL;
  int rounds_left = 0;
  float fTime = 0.0;
  // Keyboard timing
  float fNextKeyTime = 0.0;

  void MainLoop();

public:
  DestructionClient() {
    sAppName = "Destructed earth";

    if (pge != NULL) delete pge;
    pge = this;
  }

  bool OnUserCreate() override {
    running = true;

    srand(time(NULL));

    running &= Init();

    menu = new MainMenu();

    return running;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    fTime += fElapsedTime;
    if (GetKey(olc::Key::ESCAPE).bPressed) {
      if (game_round != NULL) game_round->refresh = true;
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
    if (game_round != NULL) delete game_round;
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
  } else if (game_round != NULL) {
    DrawRound_Clear();
    CleanObjects();
    game_round->Update(ReadKeys(this));
    DrawRound_Draw();
    if (game_round->state == GameRound::END) {
      rounds_left--;
      Map_DestroyMap();
      delete game_round;
      game_round = NULL;
    }
  } else if (rounds_left > 0) {
    game_round = new GameRound(WIDTH - 4, HEIGHT - 5, players, rounds_left);
    game_round->CreateRound();
  } else {
    menu = new MainMenu();
  }
}

int main(int args, char** argv) {
  DestructionClient cli;
  if (cli.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) cli.Start();
  return 0;
}

