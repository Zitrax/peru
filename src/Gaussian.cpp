/*************************************************

 Purpose:
   This class implements the Gaussian noise 
   removal filter

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: Gaussian.cpp,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

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
