/*************************************************

 Purpose:
   This filter corrects for perspective
   I.etc. stretches object far away

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: PerspectiveCorrect.h,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

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
