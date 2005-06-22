/*************************************************

 Purpose:
   This filter removes gradients in images
   Purpose in peruapplication is to eliminate
   the gradients left by the blockmatching 
   algorithms.

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: GradientRemoval.h,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

*************************************************/

#ifndef GRADIENTREMOVAL_H
#define GRADIENTREMOVAL_H

#include "Filter.h"
#include "ccv.h"

class GradientRemoval : public Filter
{
public:
  
  GradientRemoval();
  void apply(IplImage* image);

};

#endif
