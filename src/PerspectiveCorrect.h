/*************************************************

 Purpose:
   This filter corrects for perspective
   I.etc. stretches object far away

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: PerspectiveCorrect.h,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

*************************************************/

#ifndef PERSPECTIVECORRECT_H
#define PERSPECTIVECORRECT_H

#include "ccv.h"
#include "Filter.h"

class PerspectiveCorrect : public Filter
{

public:
  
  PerspectiveCorrect();
  void apply(IplImage* image);
  
};

#endif
