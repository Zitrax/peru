/*************************************************

 Purpose:
   Subclass of Stereo
   Implements a blockmatching stereo algorithm

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: BlockMatch.cpp,v 1.2 2003/09/05 12:15:12 cygnus78 Exp $

*************************************************/

#include "BlockMatch.h"

BlockMatch::BlockMatch(string left, string right, string out) 
  : Stereo(left, right, out) {}
BlockMatch::BlockMatch(string left, string right, string out, 
		       bool c, int maxd, int bls, bool f, bool sq) 
  : Stereo(left, right, out) { MAXD = maxd; color = c; 
                               fast = f; square=sq; bs=bls;}

bool
BlockMatch::calculateDisparityFast()
{
  int nc;
  if(!color) nc = 1;
  else nc = leftI->nChannels;

  int* lC[width+2];
  int* rC[width+2];

  for(int t=0; t<width+2; t++){
    lC[t] = new int[height+2];
    rC[t] = new int[height+2];
  }

  // Reset to zero sum
  for(int x=1;x<=width;x++) 
    for(int y=1;y<=height;y++) {
      lC[x][y] = 0;
      rC[x][y] = 0;
    }

  uchar* rptr;
  uchar* lptr;

  for(int x=1;x<=width;x++) {
    for(int y=1;y<=height;y++) {
      rptr = ccv::pixel_u(rightI,x-1,y-1);
      lptr = ccv::pixel_u(leftI,x-1,y-1);
      for(int c=0; c<nc; c++) {
	iterations++;
	lC[x-1][y-1]+=lptr[c];
	lC[x  ][y-1]+=lptr[c];
	lC[x+1][y-1]+=lptr[c];
	lC[x-1][y  ]+=lptr[c];
	lC[x  ][y  ]+=lptr[c];
	lC[x+1][y  ]+=lptr[c];
	lC[x-1][y+1]+=lptr[c];
	lC[x  ][y+1]+=lptr[c];
	lC[x+1][y+1]+=lptr[c];

	rC[x-1][y-1]+=rptr[c];
	rC[x  ][y-1]+=rptr[c];
	rC[x+1][y-1]+=rptr[c];
	rC[x-1][y  ]+=rptr[c];
	rC[x  ][y  ]+=rptr[c];
	rC[x+1][y  ]+=rptr[c];
	rC[x-1][y+1]+=rptr[c];
	rC[x  ][y+1]+=rptr[c];
	rC[x+1][y+1]+=rptr[c];
      }
    }
  }

  int mse; int disparity; int val; uchar value; uchar* dptr;

  zapImg(dispI);
  dispI = cvCreateImage( cvSize(rightI->width,
				rightI->height), 
			 IPL_DEPTH_8U, 1);

  // Should be changed to just border for performance
  cvSetZero( dispI );
  
  for(int x=1;x<(width-MAXD);x++) {
    for(int y=1;y<(height-1);y++) {
      mse = 1e13; disparity = 0;
      for(int offset=0;offset<MAXD;offset++) {
	val = abs(rC[x+1][y+1] - lC[x+offset+1][y+1]);
	if(val<mse) {
	  mse = val;
	  disparity = offset;
	}
      }
      //      value = static_cast<uchar>((static_cast<float>(disparity)/MAXD)*256);
      value = disparity;
      dptr = ccv::pixel_u(dispI,x,y);     
      (dptr)[0] = value;     // blue
//       (dptr)[1] = value;     // green
//       (dptr)[2] = value;     // red
    }
  }

  for(int t=0; t<width+2; t++){
    delete [] lC[t];
    delete [] rC[t];
  }

  return true;
}

bool
BlockMatch::calculateDisparity()
{
  if(ccv::debug) std::cerr << "BlockMatch::calculateDisparity\n";
  if(ccv::debug) std::cerr << "BS = " << bs << "\n";

  if(fast) { return calculateDisparityFast(); }

  if(bs%2==0) {
    if(ccv::debug) std::cerr << "Must use odd number for blocksize\n";
    return false;
  }

  zapImg(dispI);
  dispI = cvCreateImage( cvSize(rightI->width,
				rightI->height), 
			 IPL_DEPTH_8U, 1);
  cvSetZero( dispI );

  IplImage* tmpL = cvCreateImage( cvSize(leftI->width+bs-1+MAXD,
					 leftI->height+bs-1),
				  leftI->depth, leftI->nChannels); 

  IplImage* tmpR = cvCreateImage( cvSize(rightI->width+bs-1+MAXD,
					 rightI->height+bs-1),
				  rightI->depth, rightI->nChannels); 
  
  cvSetZero( tmpL );
  cvSetZero( tmpR );

  int bs2 = bs/2;

  for(int x=0; x<leftI->width; x++)
    for(int y=0; y<leftI->height; y++) 
      for(int c=0; c<leftI->nChannels; c++) {
	ccv::pixel_u(tmpL, x+bs2, y+bs2 )[c] = 
	  ccv::pixel_u(leftI, x, y )[c];
	ccv::pixel_u(tmpR, x+bs2, y+bs2 )[c] = 
	  ccv::pixel_u(rightI, x, y )[c];
      }

//   cvSaveImage("lll.bmp",tmpL);
//   cvSaveImage("rrr.bmp",tmpR);
  
  if(ccv::debug) std::cerr << "tmp images set up\n";

  for(int x=0; x<leftI->width; x++) {
    for(int y=0; y<leftI->height; y++) {
      
      int mse = 1e13; int disparity = 0;
      uchar* dptr = ccv::pixel_u(dispI,x,y);
      uchar* rptr = ccv::pixel_u(tmpR,x+bs2,y+bs2);
      uchar* lptr = ccv::pixel_u(tmpL,x+bs2,y+bs2);

      for(int offset=0;offset<MAXD;offset++) {
	
	int diffV = windowDiff(lptr, rptr, tmpL->widthStep, bs, tmpL->nChannels);
	lptr+=tmpL->nChannels;
	iterations++;
	if(diffV<mse) { mse = diffV; disparity = offset; }
      }

      //   uchar val = static_cast<uchar>((static_cast<float>(disparity)/MAXD)*256);
      uchar val = disparity;

      (dptr)[0] = val ;    // blue
//       (dptr)[1] = val;     // green
//       (dptr)[2] = val;     // red
    }
  }

  zapImg( tmpL );
  zapImg( tmpR );

  return true;
}


int
BlockMatch::windowDiff(uchar* l, uchar* r, int wstep, int n, int nc)
{
  
  int diff=0;

  int c;
  if(!color) nc=1;

  for(int x=-(n/2); x<(n/2); x++)
    for(int y=-(n/2); y<(n/2); y++)
      for(int c=0; c<nc; c++)
	if(!square)
	  diff += abs( (r+y*wstep+x*nc)[c] - (l+y*wstep+x*nc)[c] );
	else
	  diff += pow(static_cast<float>((r+y*wstep+x*nc)[c]) - (l+y*wstep+x*nc)[c], 2);

  
  return diff;
}

