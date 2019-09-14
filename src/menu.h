#ifndef MENU_H
#define MENU_h

class MainMenu {
  int frame = 0;

public:
  bool ready = false, quit = false;

  bool Update(olc::PixelGameEngine*);

  bool DrawMenu(olc::PixelGameEngine*);
};

#endif
