/*************************************************

 Purpose:
   This class implements Contrast Stretching

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: ContrastStretch.cpp,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

*************************************************/

#include "ContrastStretch.h"

ContrastStretch::ContrastStretch() : Filter() {}

void
ContrastStretch::apply(IplImage* image) {

  if(ccv::debug) std::cerr << "ContrastStretch::apply\n";
  //First find min and max value
  double* min = new double;
  double* max = new double;
  CvPoint* notused = new CvPoint;
  
  cvMinMaxLoc(image, min, max, notused, notused );
  
  float diff = *max - *min;
  
  if(ccv::debug) std::cerr << "(Min,Max,Diff) = " 
			   << *min << "," << *max << "," << diff << "\n";
  
  for(int x=0; x<image->width; x++)
    for(int y=0; y<image->height; y++)
      *ccv::pixel_u(image,x,y) = 
	static_cast<uchar>(((*ccv::pixel_u(image,x,y) - *min)/diff)*255);
  
  delete min;
  delete max;
  delete notused;
  
}
  
