#ifndef VECTOR2D_H
#define VECTOR2D_H

struct Point {
  int x = -1, y = -1;

  Point();
  Point(int, int);

  Point operator+(const Point) const;
  Point operator-(const Point) const;
};

struct Vector2D {
  float x = 0, y = 0;

  Vector2D();
  Vector2D(Point);
  Vector2D(float, float);

  Vector2D operator*(const float) const;
  Vector2D operator*=(const float);
  Vector2D operator/(const float) const;
  Vector2D operator/=(const float);
  Vector2D operator+(const Vector2D&) const;
  Vector2D operator+=(const Vector2D&);
  Vector2D operator-(const Vector2D&) const;
  Vector2D operator-=(const Vector2D&);

  float length() const;
  Vector2D unit() const;
  Vector2D reversed() const;
  float dot(const Vector2D&) const;
  Vector2D rotate(const Vector2D&);

  Point GetPoint() const;
};

Vector2D VectorFromAngle(const float, const float);
Vector2D VectorFromAngle(const int, const float);

#endif
