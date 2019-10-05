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
  int round = 0;
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

    running &= Game_Init();

    configuration.width = ScreenWidth() - 4;
    configuration.height = ScreenHeight() - 5;

    menu = new MainMenu();

    return running;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    fTime += fElapsedTime;

    // Key combination Esc + F10 should always close the game. Esc pauses the
    // game and when paused F10 closes the program
    if (GetKey(olc::Key::ESCAPE).bPressed) {
      if (game_round != NULL) game_round->refresh = true;
      paused = !paused;
      //TODO Also on main menu Esc should straight just exit the program
    }
    if (paused && GetKey(olc::Key::F10).bPressed) running = false;

    // Display text when paused
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

    return true;
  }

  void ReadKeys() {
    if (game_round->state == GameRound::SCORES and game_round->tic > 25) {
      if (pge->GetKey(olc::Key::ENTER).bPressed
          or pge->GetKey(olc::Key::RETURN).bPressed
          or pge->GetKey(olc::Key::A).bPressed) {
        game_round->state = GameRound::END;
        game_round->refresh = true;
      }
    } else {
      if (game_round->selected_tank == NULL) return;
      Tank* tnk = game_round->selected_tank;
      if (tnk->player->type == Player::HUMAN) {
        if (pge->GetKey(olc::Key::SPACE).bPressed) tnk->ready = true;

        if (fTime < fNextKeyTime) return;

        int spd = 1;
        if (pge->GetKey(olc::Key::CTRL).bHeld)  spd *= 10;
        if (pge->GetKey(olc::Key::SHIFT).bHeld) spd *= 100;

        if (pge->GetKey(olc::Key::RIGHT).bHeld) {
          tnk->angle += spd;
          fNextKeyTime = fTime + KEY_TIME;
        } else if (pge->GetKey(olc::Key::LEFT).bHeld) {
          tnk->angle -= spd;
          fNextKeyTime = fTime + KEY_TIME;
        } if (pge->GetKey(olc::Key::UP).bHeld) {
          tnk->power += spd;
          fNextKeyTime = fTime + KEY_TIME;
        } else if (pge->GetKey(olc::Key::DOWN).bHeld) {
          tnk->power -= spd;
          fNextKeyTime = fTime + KEY_TIME;
        }
      }
    }
  }
};

void DestructionClient::MainLoop() {
  if (menu != NULL) {
    menu->Update(this);
    menu->DrawMenu(this);
    round = 0;
    if (menu->quit) running = false;
    if (menu->ready) {
      delete menu;
      menu = NULL;
    }
  } else if (game_round != NULL) {
    DrawRound_Clear();
    CleanObjects();
    ReadKeys();
    game_round->Update();
    DrawRound_Draw();
    if (game_round->state == GameRound::END) {
      round++;
      Map_DestroyMap();
      delete game_round;
      game_round = NULL;
    }
  } else if (round <= configuration.rounds) {
    game_round = new GameRound(&configuration, round);
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

