/*************************************************

 Purpose:
   This class implements different filters that
   can be passed to Stereo class

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: Filter.h,v 1.3 2006/11/14 22:05:24 cygnus78 Exp $

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
  virtual ~Filter(){}

  virtual void apply(IplImage* image) = 0;   //!< Must be implemented in subclass

};

#endif
