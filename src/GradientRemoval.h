/*************************************************

 Purpose:
   This filter removes gradients in images
   Purpose in peruapplication is to eliminate
   the gradients left by the blockmatching 
   algorithms.

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: GradientRemoval.h,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

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
