/*************************************************

 Purpose:
   This filter removes gradients in images
   Purpose in peruapplication is to eliminate
   the gradients left by the blockmatching 
   algorithms.

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: GradientRemoval.h,v 1.3 2006/11/14 22:05:24 cygnus78 Exp $

*************************************************/

#ifndef GRADIENTREMOVAL_H
#define GRADIENTREMOVAL_H

#include "Filter.h"
#include "ccv.h"

class GradientRemoval : public Filter
{
public:
  
  GradientRemoval();
  virtual ~GradientRemoval(){}

  void apply(IplImage* image);

};

#endif
