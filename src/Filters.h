/*************************************************

 Purpose:
   This class implements different filters that
   can be passed to Stereo class

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: Filters.h,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#ifndef FILTERS_H
#define FILTERS_H

#include <opencv/cv.h>

class Filter
{
  IplImage* image

 public:
  Filters(IplImage* img);
  void apply();

};

#endif
