#include <cmath>
#include "vector.h"

const float pi  = 3.141592;
const float tau = 6.283185;

Point::Point() {}

Point::Point(const int x, const int y) {
  this->x = x; this->y = y;
}

Point Point::operator+(const Point other) {
  return { x + other.x, y + other.y };
}

Point Point::operator-(const Point other) {
  return { x - other.x, y - other.y };
}


Vector2D::Vector2D() {}

Vector2D::Vector2D(const Point pt) {
  x = pt.x; y = pt.y;
}

Vector2D::Vector2D(const float x, const float y) {
  this->x = x; this->y = y;
}

Vector2D Vector2D::Vector2D::operator*(const float mult) {
  return { x * mult, y = mult };
}

Vector2D Vector2D::Vector2D::operator*=(const float mult) {
  x *= mult; y *= mult;
  return *this;
}

Vector2D Vector2D::operator/(const float div) {
  return { x / div, y / div };
}

Vector2D Vector2D::operator/=(const float div) {
  x /= div; y /= div;
  return *this;
}

Vector2D Vector2D::operator+(const Vector2D& other) {
  return { x + other.x, y + other.y };
}

Vector2D Vector2D::operator+=(const Vector2D& other) {
  x += other.x; y += other.y;
  return *this;
}

Vector2D Vector2D::operator-(const Vector2D& other) {
  return { x - other.x, y - other.y };
}

Vector2D Vector2D::operator-=(const Vector2D& other) {
  x -= other.x; y -= other.y;
  return *this;
}

float Vector2D::length() {
  return std::sqrt(x * x + y * y);
}

Vector2D Vector2D::unit() {
  float l = length();
  return { x / l, y / l };
}

Vector2D Vector2D::reversed() {
  return { -x, -y };
}

float Vector2D::dot(const Vector2D& other) {
  return x * other.x + y * other.y;
}

Vector2D Vector2D::rotate(const Vector2D& other) {
  return { x * other.y - y * other.x, x * other.x + y * other.y };
}

Point Vector2D::GetPoint() {
  return { (int)x, (int)y };
}


Vector2D VectorFromAngle(float angle, float size) {
  return { std::sin(angle) * size, -std::cos(angle) * size };
}

Vector2D VectorFromAngle(int angle, float size) {
  float a = (float)angle * pi / 180.0;
  return { std::sin(a) * size, -std::cos(a) * size };
}

