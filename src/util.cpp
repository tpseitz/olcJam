#include <iostream>
#include <fstream>
#include "util.h"

void Log(int level, std::string message) {
  if (level < 2) std::cerr << message << std::endl;
  else std::cout << message << std::endl;
}

Range::Range(int start, int end) {
  s = start; e = end;
}

void Range::Update(int start, int end) {
  s = std::min(start, s);
  e = std::max(end, e);
}

Area::Area(int lx, int ly, int width, int height) {
  x = lx; y = ly; w = width; h = height;
}

Area::Area(Point sp, Point ep) {
  int sx = sp.x, sy = sp.y, ex = ep.x, ey = ep.y;
  if (sx > ex) { std::swap(sx, ex); }
  if (sy > ey) { std::swap(sy, ey); }
  x = sx; y = sy; w = ex - sx; h = ey - sy;
}

Area Area::operator+(const Point pt) const {
  return { x + pt.x, y + pt.y, w, h };
}

Area Area::operator+=(const Point pt) {
  x += pt.x; y += pt.y;
  return *this;
}

Area Area::operator-(const Point pt) const {
  return { x - pt.x, y - pt.y, w, h };
}

Area Area::operator-=(const Point pt) {
  x -= pt.x; y -= pt.y;
  return *this;
}


std::string Strip(const std::string str) {
  int len = str.size();
  int s = 0, e = len - 1;
  while (str[s] < '!' and s < len) s++;
  while (str[e] < '!' and e > s) e--;

  if (s > e) return std::string("");

  return str.substr(s, e - s + 1);
}

std::vector<std::string> ReadLines(const std::string filename) {
  std::ifstream file;
  file.open(filename.c_str());
  if (!file) {
    Log(0, "Could not open file");
    return std::vector<std::string>();
  }

  std::vector<std::string> lines;
  std::string ln;
  while (getline(file, ln)) {
    ln = Strip(ln);
    if (ln.size() > 0) lines.push_back(ln);
  }
  file.close();

  return lines;
}

std::string RandomChoice(
    const std::vector<std::string> list, std::set<int> *exclude) {
  if (list.empty()) return std::string("");

  if (exclude == NULL) return list[rand() % list.size()];

  int tries = list.size() * 2;
  for (int c = 0; c < tries; c++) {
    int i =  rand() % list.size();
    if (exclude->find(i) == exclude->end()) {
      exclude->insert(i);
      return list[i];
    }
  }

  return list[rand() % list.size()];
}


Fraction::Fraction(int num, int den) {
  numerator = num; denominator = den;
  if (denominator == 0) ; //TODO Cause exception
}

int Fraction::operator*(int other) const {
  return other * numerator / denominator;
}

int Fraction::operator/(int other) const {
  return other * denominator / numerator;
}

