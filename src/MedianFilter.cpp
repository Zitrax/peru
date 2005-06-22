/*************************************************

 Purpose:
   This class implements the median noise 
   removal filter

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: MedianFilter.cpp,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

*************************************************/

#include "MedianFilter.h"

MedianFilter::MedianFilter(int size) : Filter(),size(size) {}

void
MedianFilter::apply(IplImage* image)
{
  if(ccv::debug) std::cerr << "Applying median filter\n";
  IplImage* tmpD;
  tmpD = cvCloneImage( image );  

  cvSmooth(tmpD, image, CV_MEDIAN, size);
  
  cvReleaseImage(&tmpD);
}
