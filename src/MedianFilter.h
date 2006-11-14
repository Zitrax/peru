/*************************************************

 Purpose:
   This class implements the median noise 
   removal filter

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: MedianFilter.h,v 1.3 2006/11/14 22:05:24 cygnus78 Exp $

*************************************************/

#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

#include "Filter.h"

class MedianFilter : public Filter
{
  int size;

 public:
  MedianFilter(int size=3);
  virtual ~MedianFilter(){}

  void apply(IplImage* image);

};

#endif
