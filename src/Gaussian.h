/*************************************************

 Purpose:
   This class implements the gaussian noise 
   removal filter

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: Gaussian.h,v 1.3 2006/11/14 22:05:24 cygnus78 Exp $

*************************************************/

#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include "Filter.h"

class Gaussian : public Filter
{
  int size;

 public:
  Gaussian(int size=3);
  virtual ~Gaussian(){}

  void apply(IplImage* image);

};

#endif
