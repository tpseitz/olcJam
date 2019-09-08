#include "game.h"
#include "menu.h"

const uint32_t DASH_BITS = 0b11001100110011001100110011001100;

bool DrawHelp(olc::PixelGameEngine* pge) {
  int sx = 16, sy = 144, lh = 24, i = 0;
  pge->DrawString(sx, sy + i++ * lh, "Player types", olc::WHITE, 3);
  pge->DrawLine(sx - 2, sy + i * lh,   sx + 288, sy + i * lh,   olc::WHITE, DASH_BITS);
  i++;
  pge->DrawString(sx, sy + i++ * lh, "Click player to change type", olc::WHITE, 2);
  pge->DrawString(sx, sy + i++ * lh, "H: Human player", olc::WHITE, 2);
  pge->DrawString(sx, sy + i++ * lh, "R: Random bot", olc::WHITE, 2);
  i++;
  pge->DrawString(sx, sy + i++ * lh, "How the game works", olc::WHITE, 3);
  pge->DrawLine(sx - 2, sy + i * lh, sx + 432, sy + i * lh, olc::WHITE, DASH_BITS);
  i++;
  pge->DrawString(sx, sy + i++ * lh, "up and down increases and decreases power", olc::WHITE, 2);
  pge->DrawString(sx, sy + i++ * lh, "left and right changes angle", olc::WHITE, 2);
  pge->DrawString(sx, sy + i++ * lh, "Ctrl multiplies changes by 10", olc::WHITE, 2);
  pge->DrawString(sx, sy + i++ * lh, "shift multiplies changes by 100", olc::WHITE, 2);
  pge->DrawString(sx, sy + i++ * lh, "space bar ends turn", olc::WHITE, 2);

  return true;
}

bool MainMenu::Update(olc::PixelGameEngine* pge) {
  int mx = pge->GetMouseX(), my = pge->GetMouseY();
  olc::HWButton mb1 = pge->GetMouse(0);

  if (mb1.bPressed) {
    if (my >= 13 and my <= 35) {
      if      (mx >= 138 and mx <= 160) game_rounds--;
      else if (mx >= 234 and mx <= 256) game_rounds++;
      else if (mx >= 426 and mx <= 448 and players.size() > 2) {
        Player* plr = players.back();
        players.pop_back();
        delete plr;
      }
      else if (mx >= 506 and mx <= 528 and players.size() < MAX_PLAYERS)
        players.push_back(
          new Player("Player " + std::to_string(players.size()),
            COLOR_LIST[players.size() % COLOR_COUNT], HUMAN));
    } else if (my >= 48 and my <= 72) {
      for (int i = 0; i < players.size(); i++) {
        int x = 16 + 32 * i;
        if (mx >= x and mx <= x + 24) {
          players[i]->SwitchType();
          break;
        }
      }
    } else if (my >= 88 and my <= 112) {
      if      (mx >= 16 and mx <= 80) {
        for (Player* plr: players) plr->score = 0;
        ready = true;
      } else if (mx >= 96 and mx <= 192) {
        quit = true; ready = true;
      }
    }
  }

  if (game_rounds < 1) game_rounds = 1;
  if (game_rounds > 100) game_rounds = 100;

  return true;
}

bool MainMenu::DrawMenu(olc::PixelGameEngine* pge) {
  if (frame == 0)
    pge->FillRect(0, 0, pge->ScreenWidth(), pge->ScreenHeight(), olc::BLACK);

  pge->FillRect(16, 16, 512, 16, olc::BLACK);
  pge->FillRect(15, 47, 32 * (players.size() + 1), 26, olc::BLACK);
  pge->FillRect(15, 87, 176, 25, olc::BLACK);

  int mx = pge->GetMouseX(), my = pge->GetMouseY();
  if (my >= 13 and my <= 35) {
    if      (mx >= 138 and mx <= 160)
      pge->FillRect(138, 13, 22, 19, olc::DARK_GREY);
    else if (mx >= 234 and mx <= 256)
      pge->FillRect(234, 13, 22, 19, olc::DARK_GREY);
    else if (mx >= 426 and mx <= 448)
      pge->FillRect(426, 13, 22, 19, olc::DARK_GREY);
    else if (mx >= 506 and mx <= 528)
      pge->FillRect(506, 13, 22, 19, olc::DARK_GREY);
  } else if (my >= 48 and my <= 72) {
    for (int i = 0; i < players.size(); i++) {
      int x = 16 + 32 * i;
      if (mx >= x and mx <= x + 24) {
        pge->FillRect(x, 48, 24, 24, olc::VERY_DARK_GREY);
        break;
      }
    }
  } else if (my >= 88 and my <= 112) {
    if      (mx >= 16 and mx <= 80)
      pge->FillRect(16, 88, 64, 24, olc::DARK_GREY);
    else if (mx >= 96 and mx <= 192)
      pge->FillRect(96, 88, 96, 24, olc::DARK_GREY);
  }

  char buf[34] = { 0 };
  sprintf(buf,"Rounds: < %3d >  Players: < %2d >",game_rounds, players.size());
  pge->DrawString(16, 16, buf, olc::WHITE, 2);

  pge->DrawRect(138, 13, 22, 19, olc::WHITE);
  pge->DrawRect(234, 13, 22, 19, olc::WHITE);
  pge->DrawRect(426, 13, 22, 19, olc::WHITE);
  pge->DrawRect(506, 13, 22, 19, olc::WHITE);

  int i = 0, y = 48, x;
  for (Player* plr: players) {
    x = 16 + i++ * 32;
    switch (plr->type) {
      case HUMAN:  pge->DrawString(x + 5, y + 5, "H", plr->color, 2); break;
      case RANDOM: pge->DrawString(x + 5, y + 5, "R", plr->color, 2); break;
      default: pge->DrawCircle(x + 12, y + 12, 8, olc::WHITE);
    }
    pge->DrawRect(x, y, 24, 24, olc::WHITE);
  }

  pge->DrawString(32, 94, "OK   Quit", olc::WHITE, 2);
  pge->DrawRect(16, 88, 64, 24, olc::WHITE);
  pge->DrawRect(96, 88, 96, 24, olc::WHITE);

  DrawHelp(pge);

  frame++;
  return true;
}

