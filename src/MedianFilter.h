/*************************************************

 Purpose:
   This class implements the median noise 
   removal filter

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: MedianFilter.h,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

#include "Filter.h"

class MedianFilter : public Filter
{
  int size;

 public:
  MedianFilter(int size=3);
  void apply(IplImage* image);

};

#endif
