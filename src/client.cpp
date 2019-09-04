#include "olcPixelGameEngine.h"
#include "game.h"

const olc::Pixel COLOR_LIST[] = { 0xff0000c0, 0xffe00000, 0xff00c000,
  0xff00b0c0, 0xffc0c000, 0xffc000c0, 0xffc0c0c0, 0xff3060c0, 0xff606060,
  0xff002860 };
const int COLOR_COUNT = 10;

const int WIDTH = 1200, HEIGHT = 900, PIXEL_SIZE = 1;

class DestructionClient : public olc::PixelGameEngine {
public:
  bool running = false, paused = false;
  std::vector<Player*> players;
  GameRound* round = NULL;

  DestructionClient() {
    sAppName = "Destructed earth";
  }

  bool OnUserCreate() override {
    running = true;

    for (olc::Pixel col: COLOR_LIST) players.push_back(new Player(col, RANDOM)); //XXX
    players[0]->type = HUMAN;

    return running;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    if (GetKey(olc::Key::ESCAPE).bPressed) {
      if (round != NULL) round->refresh = true;
      paused = !paused;
    }
    if (paused && GetKey(olc::Key::F10).bPressed) running = false;

    if (paused) {
      int tx = WIDTH / 2 - 240, ty = HEIGHT / 2 - 16;
      FillRect(tx - 8, ty - 8, 560, 48, olc::BLACK);
      DrawString(tx, ty, "Press F10 to exit", { 0xffffffff }, 4);
    } else {
      if (round != NULL) {
        round->ClearRound(this);
        round->Update(this);
        round->DrawRound(this);
        if (round->state == GameRound::END) {
          delete round;
          round = NULL;
        }
      } else { //XXX
        round = new GameRound(WIDTH, HEIGHT, players);
        round->CreateRound();
      }
    }

    return running;
  }

  bool OnUserDestroy() override {
    if (round != NULL) delete round;

    for (Player* pp: players) delete pp;

    return true;
  }
};

int main() {
  DestructionClient cli;
  if (cli.Construct(WIDTH, HEIGHT, PIXEL_SIZE, PIXEL_SIZE)) cli.Start();
  return 0;
}

