/*************************************************

 Purpose:
   This class implements the median noise 
   removal filter

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: MedianFilter.cpp,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

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
