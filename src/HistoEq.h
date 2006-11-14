/*************************************************

 Purpose:
   This class implements Histogram equalization

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: HistoEq.h,v 1.4 2006/11/14 22:05:24 cygnus78 Exp $

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
  virtual ~HistoEq(){}

  void apply(IplImage* image);
  
};

#endif
