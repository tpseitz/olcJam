#include "olcPixelGameEngine.h"
#include "game.h"
#include "menu.h"

const int WIDTH = 1200, HEIGHT = 900, PIXEL_SIZE = 1;
const float KEY_TIME = 0.1;

class DestructionClient : public olc::PixelGameEngine {
  bool running = false, paused = false;
  GameRound* round = NULL;
//  Menu* menu = new MainMenu();
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

    srand (time(NULL));

    for (int i = 0; i < 2; i++)
      players.push_back(new Player("Player " + std::to_string(i + 1),
        COLOR_LIST[i % COLOR_COUNT], HUMAN));
//    players[0]->type = HUMAN; //XXX
    for (Player* plr: players) std::cout << plr->name << std::endl;

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
    round->ClearRound(this);
    round->Update(ReadKeys(this));
    round->DrawRound(this);
    if (round->state == GameRound::END) {
      rounds_left--;
      delete round;
      round = NULL;
    }
  } else if (rounds_left > 0) {
    round = new GameRound(WIDTH, HEIGHT, players, rounds_left);
    round->CreateRound();
  } else {
    menu = new MainMenu();
  }
}

int main() {
  DestructionClient cli;
  if (cli.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) cli.Start();
  return 0;
}

