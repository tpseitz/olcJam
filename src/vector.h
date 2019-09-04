#ifndef VECTOR2D_H
#define VECTOR2D_H

struct Point {
  int x = -1, y = -1;

  Point();
  Point(int, int);

  Point operator+(const Point);
  Point operator-(const Point);
};

struct Vector2D {
  float x = 0, y = 0;

  Vector2D();
  Vector2D(Point);
  Vector2D(float, float);

  Vector2D operator*(const float);
  Vector2D operator*=(const float);
  Vector2D operator/(const float);
  Vector2D operator/=(const float);
  Vector2D operator+(const Vector2D&);
  Vector2D operator+=(const Vector2D&);
  Vector2D operator-(const Vector2D&);
  Vector2D operator-=(const Vector2D&);

  float length();
  Vector2D unit();
  Vector2D reversed();
  float dot(const Vector2D&);
  Vector2D rotate(const Vector2D&);

  Point GetPoint();
};

Vector2D VectorFromAngle(float, float);
Vector2D VectorFromAngle(int, float);

#endif
