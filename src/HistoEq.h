/*************************************************

 Purpose:
   This class implements Histogram equalization

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: HistoEq.h,v 1.3 2005/06/22 23:14:24 cygnus78 Exp $

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
