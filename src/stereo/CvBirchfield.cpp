/*************************************************

 Purpose:
   This class uses the birchfield stereo algorithm
   that comes with OpenCv 0.9.4 beta3

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: CvBirchfield.cpp,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include "CvBirchfield.h"

CvBirchfield::CvBirchfield(int argc, char** argv) 
  : Stereo(argc, argv) 
{ if(ccv::debug) std::cerr << "CvBirchfield constructor\n"; }

bool
CvBirchfield::calculateDisparity()
{
  zapImg(dispI);
  dispI = cvCreateImage( cvGetSize(right), IPL_DEPTH_8U, 1);

  IplImage* leftImage = cvCreateImage(cvGetSize(left), IPL_DEPTH_8U, 1);
  IplImage* rightImage = cvCreateImage(cvGetSize(right), IPL_DEPTH_8U, 1);

  cvCvtColor(left, leftImage, CV_BGR2GRAY);
  cvCvtColor(right, rightImage, CV_BGR2GRAY);

  cvFindStereoCorrespondence( leftImage, rightImage, CV_DISPARITY_BIRCHFIELD, 
			      dispI,  MAXD, 15, 3, 6, 8, 15 );


  cvReleaseImage(&leftImage);
  cvReleaseImage(&rightImage);

  return true;
}
