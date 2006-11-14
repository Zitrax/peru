/*************************************************

 Purpose:
   This filter corrects for perspective
   I.etc. stretches object far away

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: PerspectiveCorrect.cpp,v 1.3 2006/11/14 22:05:24 cygnus78 Exp $

*************************************************/

#include "PerspectiveCorrect.h"

PerspectiveCorrect::PerspectiveCorrect() : Filter() {}

void
PerspectiveCorrect::apply(IplImage* image)
{
  if(ccv::debug) std::cerr << "Applying perspective correction\n";

  IplImage* hresult = cvCreateImage( cvSize(image->width, image->height),
				     image->depth, image->nChannels );
  IplImage* vresult = cvCreateImage( cvSize(image->width, image->height),
				     image->depth, image->nChannels );
  IplImage* result = cvCreateImage( cvSize(image->width, image->height),
				    image->depth, image->nChannels );
  
  cvZero( result );
  cvZero( hresult );
  cvZero( vresult );

  double* min = new double;
  double* max = new double;
  CvPoint* notused = new CvPoint;
  cvMinMaxLoc(image, min, max, notused, notused );

  int stop    =  0; 
  int start   = -1;
  uchar val   =  0;
  uchar tmp   =  0;
  uchar* tmp2 =  0;
  double midpoint = 0;
  int nstop, nstart;

  // Horizontal stretch
  for(int y=0; y<image->height; y++) {
    val = *ccv::pixel_u( image, 0, y);
    start = 0;
    for(int x=1; x<image->width; x++) {
      tmp = *ccv::pixel_u( image, x, y);
      // This means that we finish one sequence
      // and thus fills hresult with the enlarged sequence
      if( tmp != val || x==(image->width-1)) {
	if( val!=0 ) {
	  stop = x-1;
	  midpoint = start + (stop-start)/2.0;
	  nstart = cvRound(midpoint - (midpoint-start)*(*max)/val);
	  if(nstart<0) nstart = 0;
	  nstop = cvRound(midpoint + (midpoint-start)*(*max)/val);
	  if(nstop>=image->width) nstop=image->width-1;
	  for(int i=0; i<(nstop-nstart+1); i++) {
	    tmp2 = ccv::pixel_u( hresult, nstart+i, y );
	    // Ensure that closer objects are in front
	    if( val > *tmp2 ) 
	      *tmp2 = val;
	  }
	}
	start = x; val = tmp;
      }
    }
  }

  // Vertical stretch
  for(int x=0; x<image->width; x++) {
    val = *ccv::pixel_u( image, x, 0);
    start = 0;
    for(int y=1; y<image->height; y++) {
      tmp = *ccv::pixel_u( image, x, y);
      // This means that we finish one sequence
      // and thus fills vresult with the enlarged sequence
      if( tmp != val || y==(image->height-1)) {
	if( val!=0 ) {
	  stop = y-1;
	  midpoint = start + (stop-start)/2.0;
	  nstart = cvRound(midpoint - (midpoint-start)*(*max)/val);
	  if(nstart<0) nstart = 0;
	  nstop = cvRound(midpoint + (midpoint-start)*(*max)/val);
	  if(nstop>=image->height) nstop=image->height-1;
	  for(int i=0; i<(nstop-nstart+1); i++) {
	    tmp2 = ccv::pixel_u( vresult, x, nstart+i );
	    // Ensure that closer objects are in front
	    if( val > *tmp2 ) 
	      *tmp2 = val;
	  }
	}
	start = y; val = tmp;
      }
    }
  }

  uchar tmpv;
  uchar tmph;	
  // Put vresult and hresult together
  for(int y=0; y<image->height; y++)
    for(int x=0; x<image->width; x++) {
      tmph = *ccv::pixel_u( hresult, x, y);
      tmpv = *ccv::pixel_u( vresult, x, y);
      *ccv::pixel_u( result, x, y) = ((tmpv>tmph) ? tmpv : tmph); 
    }

  cvCopy( result, image );

  zapImg( result );
  zapImg( hresult );
  zapImg( vresult );

  delete min;
  delete max;
  delete notused;

}
