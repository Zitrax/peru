/*************************************************

 Purpose:
   This class implements Histogram equalization

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: HistoEq.h,v 1.2 2003/09/18 21:25:55 cygnus78 Exp $

*************************************************/

#ifndef HISTOEQ_H
#define HISTOEQ_H

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
