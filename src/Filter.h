/*************************************************

 Purpose:
   This class implements different filters that
   can be passed to Stereo class

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: Filter.h,v 1.4 2007/03/22 23:42:39 cygnus78 Exp $

*************************************************/

#ifndef FILTERS_H
#define FILTERS_H

#include "ccv.h"

#include "inc/Filter_externals.h"

class Filter
{
 public:
  Filter();
  virtual ~Filter(){}

  virtual void apply(IplImage* image) = 0;   //!< Must be implemented in subclass

};

#endif
