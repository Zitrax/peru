/*************************************************

 Purpose:
   Hierarchical block stereo algorithm

 Author:
   Daniel Bengtsson 2002, daniel@bengtssons.info

 Version:
   $Id: PyramidBlock.cpp,v 1.5 2005/06/22 23:14:23 cygnus78 Exp $

*************************************************/

#include "PyramidBlock.h"

PyramidBlock::PyramidBlock(string left, string right, string out,
			   int maxd, int nlevels, int tol, bool c) 
  : Stereo(left, right, out), levels(nlevels), color(c), MAXD(maxd)
{
  // Should change nr of lvls as we do not look at lvl+1

  error = false;

  tolerance = new int[levels+1];
  tolerance[0] = tol;
  for( int i=1; i<=levels; i++) {
    tolerance[i]=tolerance[i-1]/2;
    if(tolerance[i]==0) tolerance[i]=1;
  }

  pyramid_left  = new IplImage*[levels+1];
  pyramid_right = new IplImage*[levels+1];

  pyr_disp = new int**[levels+2];

  CvSize size = imageSize;

  for( int i=1; i<=levels+1; i++) {
    size.width/=2; size.height/=2;
    pyr_disp[i] = new int*[size.width];
    for( int x=0; x<size.width; x++)
      pyr_disp[i][x] = new int[size.height];
  }

  if(ccv::debug) std::cerr << "Finished Pyramid constructor\n";

}

PyramidBlock::~PyramidBlock() 
{
  if(ccv::debug) std::cerr << "pyramid destructor\n";

  // Free memory
  // level 0 is released in stereo
  if(!error) {
    for(int i=1; i<=levels; i++ ){
      zapImg( pyramid_left [i] );
      zapImg( pyramid_right[i] );
    }
  }

  if(ccv::debug) std::cerr << "released images\n";

  CvSize size = imageSize;
  if(ccv::debug) std::cerr << "width = " << size.width << endl;
  for( int i=1; i<=levels+1; i++) {
    size.width/=2; size.height/=2;
    for( int x=0; x<size.width; x++ )
      delete [] pyr_disp[i][x];
    delete [] pyr_disp[i];
  }  

  if(ccv::debug) std::cerr << "deleted pyr_disp\n";

  delete [] tolerance;
  delete [] pyr_disp;
  delete [] pyramid_left;
  delete [] pyramid_right;
}

bool
PyramidBlock::calculateDisparity()
{
  if(ccv::debug) std::cerr << "PyramidBlock::calculateDisparity\n";
  
  CvSize size = imageSize;

  int tmpw = size.width; int tmph = size.height;
  for(int i=0; i<levels; i++) {
    tmpw/=2; tmph/=2;
    if(tmpw<1 || tmph<1) {
      ccv::ERRFLAG = true;
      ccv::ERROR_MESSAGE = "ERROR - Too many levels\n";
      if(ccv::debug) std::cerr << ccv::ERROR_MESSAGE;
      error = true;
      return false;
    }
  }

  zapImg(dispI);
  dispI = cvCreateImage( cvGetSize(rightI), IPL_DEPTH_8U, 1);
  cvSetZero( dispI );

  pyramid_left [0] = leftI;
  pyramid_right[0] = rightI;

  //  char* file = new char[20];

  // Create the two pyramids
  for(int i=1; i<=levels; i++ ){
    size.width/=2; size.height/=2;

    pyramid_left [i] = cvCreateImage( size, IPL_DEPTH_8U, 3);
    pyramid_right[i] = cvCreateImage( size, IPL_DEPTH_8U, 3);
    cvPyrDown( pyramid_left [i-1], pyramid_left [i], IPL_GAUSSIAN_5x5);
    cvPyrDown( pyramid_right[i-1], pyramid_right[i], IPL_GAUSSIAN_5x5);
    //     sprintf( file, "p%d.bmp", i);
    //     cvvSaveImage( file, pyramid_left[i] );
  }

  //  delete [] file;

  if(ccv::debug) std::cerr << "Finished pyr_calc_disp\n";

  for( int i=levels; i>=0; i-- )
    blockmatch(i);

  return true;
}

void
PyramidBlock::blockmatch(int level)
{
  if(ccv::debug) std::cerr << "Blockmatch on level " << level << endl;

  uchar val;
  int wStep = pyramid_right[level]->widthStep;
  double mse;
  int gmaxd,maxd,mind;
  int disparity,diffV,rightV;

  CvSize currSize = cvGetSize( pyramid_right[level] );
  uchar* rptr;
  uchar* lptr;
  
  gmaxd = static_cast<int>(MAXD/pow(2.0,level));

  if(ccv::debug) std::cerr << "gmaxd = " << gmaxd << endl;
  if(ccv::debug) std::cerr << "width = " << currSize.width << endl;
  if(ccv::debug) std::cerr << "height = " << currSize.height << endl;

  // X00  Neighbourhood used (should maybe be changed to center-pix)
  // 000
  // 000


  for(int x=0;x<(currSize.width-gmaxd);x++) {
    for(int y=0;y<currSize.height;y++) {     // This must be taken care of !!!!!!!!
      if(ccv::debug) std::cerr << "x,y=" << x << "," << y << endl;
      mse = 1e13; disparity = 0;
      // Getting pixel at x,y
      rptr = (uchar*)(pyramid_right[level]->imageData + y*wStep + x*3);
      lptr = (uchar*)(pyramid_left [level]->imageData + y*wStep + x*3);
      
      //-------INNER PART
      if(x!=currSize.width-gmaxd-1 &&
	 x!=0                      &&
	 y!=0                      &&
	 y!=currSize.height-1) {

	if(ccv::debug) std::cerr << "inner\n";

	if(level!=levels){
	  maxd = 
	    2*(pyr_disp[level+1][x/2+0][y/2] + 
	       pyr_disp[level+1][(x/2-1>=0) ? x/2-1 : 0 ][y/2] + 
	       pyr_disp[level+1]
	       [(x/2+1<currSize.width/2) ? x/2+1 : x/2][y/2])/3 + tolerance[level];
	  if(ccv::debug) std::cerr << "Setting max,min\n";
	  if( maxd>gmaxd ) maxd=gmaxd;
	  if( maxd<0 ) maxd=0; // NNN
	  mind = maxd-2*tolerance[level];
	  if( mind < 0 ) mind = 0;

	  if(ccv::debug) std::cerr << "(" << mind << "," << maxd << ") \n";

	  lptr+=leftI->nChannels*mind;
	}
	else { maxd = gmaxd; mind = 0; }
	
	for(int offset=mind;offset<maxd;offset++) {
	  //	  if(ccv::debug) std::cerr << "offset = " << offset << endl;

	diffV = 
	  abs( (rptr-wStep-3)[0] - (lptr-wStep-3)[0] ) +
	  abs( (rptr-wStep+0)[0] - (lptr-wStep+0)[0] ) +
	  abs( (rptr-wStep+3)[0] - (lptr-wStep+3)[0] ) +
	  abs( (rptr      -3)[0] - (lptr      -3)[0] ) +
	  abs( (rptr      +0)[0] - (lptr      +0)[0] ) +
	  abs( (rptr      +3)[0] - (lptr      +3)[0] ) +
	  abs( (rptr+wStep-3)[0] - (lptr+wStep-3)[0] ) +
	  abs( (rptr+wStep+0)[0] - (lptr+wStep+0)[0] ) +
	  abs( (rptr+wStep+3)[0] - (lptr+wStep+3)[0] );
	  
	if(leftI->nChannels > 1 && color) {
	  diffV += 
	    abs( (rptr-wStep-3)[1] - (lptr-wStep-3)[1] ) +
	    abs( (rptr-wStep+0)[1] - (lptr-wStep+0)[1] ) +
	    abs( (rptr-wStep+3)[1] - (lptr-wStep+3)[1] ) +
	    abs( (rptr      -3)[1] - (lptr      -3)[1] ) +
	    abs( (rptr      +0)[1] - (lptr      +0)[1] ) +
	    abs( (rptr      +3)[1] - (lptr      +3)[1] ) +
	    abs( (rptr+wStep-3)[1] - (lptr+wStep-3)[1] ) +
	    abs( (rptr+wStep+0)[1] - (lptr+wStep+0)[1] ) +
	    abs( (rptr+wStep+3)[1] - (lptr+wStep+3)[1] );
	  
	  diffV += 
	    abs( (rptr-wStep-3)[2] - (lptr-wStep-3)[2] ) +
	    abs( (rptr-wStep+0)[2] - (lptr-wStep+0)[2] ) +
	    abs( (rptr-wStep+3)[2] - (lptr-wStep+3)[2] ) +
	    abs( (rptr      -3)[2] - (lptr      -3)[2] ) +
	    abs( (rptr      +0)[2] - (lptr      +0)[2] ) +
	    abs( (rptr      +3)[2] - (lptr      +3)[2] ) +
	    abs( (rptr+wStep-3)[2] - (lptr+wStep-3)[2] ) +
	    abs( (rptr+wStep+0)[2] - (lptr+wStep+0)[2] ) +
	    abs( (rptr+wStep+3)[2] - (lptr+wStep+3)[2] );
	}
	
	  iterations++;
	  if(diffV<mse) { mse = diffV; disparity = offset; }
	  
	  lptr+=leftI->nChannels;
	}
	
// 	if(ccv::debug) std::cerr 
// 	  << "disp " << disparity 
// 	  << " on level " << level << endl;
	
	if(level==0) {
	  
	  uchar* dptr = ccv::pixel_u(dispI, x, y);

	  //val = static_cast<uchar>((static_cast<float>(disparity)/MAXD)*256);
	  val = disparity;

	  (dptr)[0] = val;    // blue
// 	  (dptr)[1] = val;     // green
// 	  (dptr)[2] = val;     // red
	  
	}
	else {
	  pyr_disp[level][x][y]=disparity;
	}
      }
    }
  }
}

