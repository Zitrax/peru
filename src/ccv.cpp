/*************************************************

 Purpose:
   This file contains the ccv namespace which
   contains useful constructions and methods

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: ccv.cpp,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include "ccv.h"

namespace ccv {
  
  int debug = 0;
  
  uchar*
  pixel_u(IplImage* image,int x, int y)
  {
    return (uchar*)(image->imageData + y*image->widthStep + x*image->nChannels);
  }
  
  float*
  pixel_f(IplImage* image,int x, int y)
  {
    return (float*)(image->imageData + y*image->widthStep + x*image->nChannels);
  }
  
  void
  contrastStretch(IplImage* img)
  {
    if(ccv::debug) std::cerr << "ccv::contrastStretch\n";
    //First find min and max value
    double* min = new double;
    double* max = new double;
    CvPoint* notused = new CvPoint;
    
    cvMinMaxLoc(img, min, max, notused, notused );
    
    float diff = *max - *min;
    
    if(ccv::debug) std::cerr << "(Min,Max,Diff) = " 
			     << *min << "," << *max << "," << diff << "\n";
    
    for(int x=0; x<img->width; x++)
      for(int y=0; y<img->height; y++)
	*ccv::pixel_u(img,x,y) = 
	  static_cast<uchar>(((*ccv::pixel_u(img,x,y) - *min)/diff)*255);
    
    delete min;
    delete max;
    delete notused;
  }
  
  
}

std::ostream& operator<<(std::ostream& os, const CvMat& m)
{
  for(int c=0; c<m.cols; c++) {
    for(int r=0; r<m.rows; r++)
      os << CV_MAT_ELEM( m, float, r, c ) << "\t";
    os << "\n";
  }
  return os;

}

