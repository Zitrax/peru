/*************************************************

 Purpose:
   This class implements Contrast Stretching

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: ContrastStretch.h,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#ifndef CONTRASTSTRETCH_H
#define CONTRASTSTRETCH_H

#include "ccv.h"
#include "Filter.h"

class ContrastStretch : public Filter
{
  
public:
  ContrastStretch();
  void apply(IplImage* image);
  
};

#endif
