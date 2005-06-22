/*************************************************

 Purpose:
   This class implements the median noise 
   removal filter

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: MedianFilter.h,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

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
