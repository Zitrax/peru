/*************************************************

 Purpose:
   This header declares all Pixel classes used
   throughout Peru

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: pixels.h,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include <iostream>
#include <opencv/cv.h>

/*! Base class of all pixels to be used in the matching */
class Pixel {
 public:
  Pixel(int x=0, int y=0, int z=0);
  
  int x;
  int y;
  int z;
  
  // Overload operators to work on pixels
  friend Pixel operator+(const Pixel& p1, const Pixel& p2);
  friend Pixel operator-(const Pixel& p1, const Pixel& p2);
  void operator+=(const Pixel& p);
  Pixel operator-();
  friend std::ostream& operator<<(std::ostream& os, const Pixel& p);
};

/*! This class contains pixels with floating point values instead of ints */
class FPixel : public Pixel {
 public:
  FPixel(float x=0, float y=0, float z=0);
  
  float x;
  float y;
  float z;
  
  friend FPixel operator+(const FPixel& p1, const FPixel& p2);
  void operator+=(const FPixel& p);
  void operator+=(const Pixel& p);
  void operator/=(int nr);
  FPixel operator-();
  friend FPixel operator-(const FPixel& p1, const FPixel& p2);
  friend std::ostream& operator<<(std::ostream& os, const FPixel& p);
};

/*! Special pixel class containg a fourth value, distance used in distmap */
//! SPixel also contains info for a source coordinate sx,sy,sz plus a distance
class SPixel : public Pixel {
 public:
  SPixel(int x=0, int y=0, int z=0, float distance=0);
  
  Pixel spixel;
  
  float distance;
  
  bool operator<(const SPixel& p) const;
  bool operator>(const SPixel& p) const;
  friend std::ostream& operator<<(std::ostream& os, const SPixel& p);
};

//! Used in priority queue of Pixel pointers
class PixelPointerCompare {
 public:
  bool operator()(const SPixel* p1, const SPixel* p2)
    { return (*p1 > *p2); }
};

