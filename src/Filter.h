/*************************************************

 Purpose:
   This class implements different filters that
   can be passed to Stereo class

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: Filter.h,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#ifndef FILTERS_H
#define FILTERS_H

#include "ccv.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

class Filter
{
 public:
  Filter();
  virtual void apply(IplImage* image) = 0;   //!< Must be implemented in subclass

};

#endif
