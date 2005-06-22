/*************************************************

 Purpose:
   This class implements the Gaussian noise 
   removal filter

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: Gaussian.cpp,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

*************************************************/

#include "Gaussian.h"

Gaussian::Gaussian(int size) : Filter(),size(size) {}

void
Gaussian::apply(IplImage* image)
{
  if(ccv::debug) std::cerr << "Applying gaussian filter\n";
  IplImage* tmpD;
  tmpD = cvCloneImage( image );  

  cvSmooth(tmpD, image, CV_GAUSSIAN, size, size);
  
  cvReleaseImage(&tmpD);
}
