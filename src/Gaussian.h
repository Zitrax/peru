/*************************************************

 Purpose:
   This class implements the gaussian noise 
   removal filter

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: Gaussian.h,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

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
