#ifndef MENU_H
#define MENU_h

#include "olcPixelGameEngine.h"

const int COLOR_COUNT = 10;
const olc::Pixel COLOR_LIST[] = { 0xff0000c0, 0xffe00000, 0xff00c000,
  0xff00b0c0, 0xffc0c000, 0xffc000c0, 0xffc0c0c0, 0xff3060c0, 0xff606060,
  0xff002860 };

class MainMenu {
  int frame = 0;

public:
  bool ready = false, quit = false;

  bool Update(olc::PixelGameEngine*);

  bool DrawMenu(olc::PixelGameEngine*);
};

#endif
