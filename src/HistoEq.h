/*************************************************

 Purpose:
   This class implements Histogram equalization

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: HistoEq.h,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#ifndef HISTOEQ_H
#define HISTOEQ_H

#include <ipl/ipl.h>
#include "Filter.h"

class HistoEq : public Filter
{
private:
  void histogramEqualize(IplImage* img);
  
public:
  HistoEq();
  void apply(IplImage* image);
  
};

#endif
