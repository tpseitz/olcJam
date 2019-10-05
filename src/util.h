#ifndef	UTIL_H
#define UTIL_H

#include <climits>
#include <string>
#include <vector>
#include <set>
#include "vector.h"

const int COLOR_COUNT = 10;
const uint32_t COLOR_LIST[] = { 0xff0000c0, 0xffe00000, 0xff00c000,
  0xff00b0c0, 0xffc0c000, 0xffc000c0, 0xffc0c0c0, 0xff3060c0, 0xff606060,
  0xff002860 };

void Log(int, std::string);

struct Range {
  int s, e;

  Range(int = INT_MIN, int = INT_MAX);
  void Update(int, int);
};

struct Area {
  int x, y, w, h;

  Area(int, int, int, int);
  Area(Point, Point);

  Area operator+(const Point) const;
  Area operator+=(const Point);
  Area operator-(const Point) const;
  Area operator-=(const Point);
};

std::string Strip(const std::string);

std::vector<std::string> ReadLines(const std::string);

std::string RandomChoice(const std::vector<std::string>, std::set<int>* =NULL);

struct Fraction {
  int numerator, denominator;

  Fraction(int, int);

  int operator*(int) const;
  int operator/(int) const;
};

#endif
