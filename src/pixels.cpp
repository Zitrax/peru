#include "pixels.h"

Pixel::Pixel(int x, int y, int z) : 
  x(x), y(y), z(z) {}

FPixel::FPixel(float x, float y, float z) 
{this->x = x; this->y = y; this->z = z;  }

SPixel::SPixel(int x, int y, int z, float distance) : 
  Pixel(x,y,z), distance(distance){}

bool
SPixel::operator<(const SPixel& p) const
{
  return (this->distance < p.distance);
}

bool
SPixel::operator>(const SPixel& p) const
{
  return (this->distance > p.distance);
}

std::ostream& operator<<(std::ostream& os, const Pixel& p){
  os << "x,y,z=(" << p.x << "," << p.y << "," << p.z << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const SPixel& p){
  os << "x,y,z,d=(" << p.x << "," << p.y << "," << p.z << "," << p.distance << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const FPixel& p){
  os << "x,y,z=(" << p.x << "," << p.y << "," << p.z << ")";
  return os;
}

Pixel
Pixel::operator-()
{
  Pixel res;
  res.x = this->x * -1;
  res.y = this->y * -1;
  res.z = this->z * -1; 

  return res;
}

FPixel
FPixel::operator-()
{
  FPixel res;
  res.x = this->x * -1;
  res.y = this->y * -1;
  res.z = this->z * -1; 

  return res;
}

Pixel 
operator+(const Pixel& p1, const Pixel& p2)
{
  Pixel res;
  res.x = p1.x+p2.x;
  res.y = p1.y+p2.y;
  res.z = p1.z+p2.z;
  return res;
}

Pixel 
operator-(const Pixel& p1, const Pixel& p2)
{
  Pixel res;
  res.x = p1.x-p2.x;
  res.y = p1.y-p2.y;
  res.z = p1.z-p2.z;
  return res;
}

FPixel 
operator+(const FPixel& p1, const FPixel& p2)
{
  FPixel res;
  res.x = p1.x+p2.x;
  res.y = p1.y+p2.y;
  res.z = p1.z+p2.z;
  return res;
}

FPixel 
operator-(const FPixel& p1, const FPixel& p2)
{
  FPixel res;
  res.x = p1.x-p2.x;
  res.y = p1.y-p2.y;
  res.z = p1.z-p2.z;
  return res;
}

void
Pixel::operator+=(const Pixel& p)
{
  this->x += p.x;
  this->y += p.y;
  this->z += p.z;
}

void
FPixel::operator+=(const Pixel& p)
{
  this->x += p.x;
  this->y += p.y;
  this->z += p.z;
}

void
FPixel::operator+=(const FPixel& p)
{
  this->x += p.x;
  this->y += p.y;
  this->z += p.z;
}

void 
FPixel::operator/=(int nr)
{
  this->x /= nr;
  this->y /= nr;
  this->z /= nr;
}
