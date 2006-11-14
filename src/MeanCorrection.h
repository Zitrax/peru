/*************************************************

 Purpose:
   This class finds the mean value of two images
   and corrects the values of so they achieve the
   same mean

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: MeanCorrection.h,v 1.3 2006/11/14 22:05:24 cygnus78 Exp $

*************************************************/

#ifndef MEANCORRECTION_H
#define MEANCORRECTION_H

#include "Filter.h"

class MeanCorrection : public Filter
{
  
private:
  IplImage** image1;
  IplImage** image2;
  bool flip; //!< Used in an evil hack

  double findMean(IplImage* img);
  void addC(IplImage* img, int value);
    
public:
  MeanCorrection(IplImage** i1, IplImage** i2);
  virtual ~MeanCorrection(){}

  void apply(IplImage* image);

};

#endif
