/*************************************************

 Purpose:
   This class implements Contrast Stretching

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: ContrastStretch.h,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

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
