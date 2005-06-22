/*************************************************

 Purpose:
   This class implements the gaussian noise 
   removal filter

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: Gaussian.h,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

*************************************************/

#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include "Filter.h"

class Gaussian : public Filter
{
  int size;

 public:
  Gaussian(int size=3);
  void apply(IplImage* image);

};

#endif
