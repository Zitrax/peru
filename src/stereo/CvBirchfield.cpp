/*************************************************

 Purpose:
   This class uses the birchfield stereo algorithm
   that comes with OpenCv 0.9.4 beta3

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: CvBirchfield.cpp,v 1.4 2005/06/22 23:14:23 cygnus78 Exp $

*************************************************/

#include "CvBirchfield.h"

CvBirchfield::CvBirchfield(string left, string right, 
			   string out, int maxd) 
  : Stereo(left, right, out) 
{
  if(ccv::debug) std::cerr << "CvBirchfield constructor\n"; 
  MAXD = maxd;
}

bool
CvBirchfield::calculateDisparity()
{
  zapImg(dispI);
  dispI = cvCreateImage( cvGetSize(rightI), IPL_DEPTH_8U, 1);

  IplImage* leftImage = cvCreateImage(cvGetSize(leftI), IPL_DEPTH_8U, 1);
  IplImage* rightImage = cvCreateImage(cvGetSize(rightI), IPL_DEPTH_8U, 1);

  cvCvtColor(leftI, leftImage, CV_BGR2GRAY);
  cvCvtColor(rightI, rightImage, CV_BGR2GRAY);

  cvFindStereoCorrespondence( leftImage, rightImage, CV_DISPARITY_BIRCHFIELD, 
			      dispI,  MAXD, 15, 3, 6, 8, 15 );


  cvReleaseImage(&leftImage);
  cvReleaseImage(&rightImage);

  return true;
}
