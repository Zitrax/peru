/*************************************************

 Purpose:
   Subclass of Stereo
   Implements a blockmatching stereo algorithm

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: BlockMatch.cpp,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include "BlockMatch.h"

BlockMatch::BlockMatch(int argc, char** argv) : Stereo(argc,argv) {}
BlockMatch::BlockMatch(int argc, char** argv, bool c, int bls, bool f, bool sq) 
  : Stereo(argc,argv) { color = c; fast = f; square=sq; bs=bls;}

// void
// BlockMatch::calculateDisparity()
// {
//   if(fast) { calculateDisparityFast(); return; }

//   uchar val;
//   int wStep = right->widthStep;
//   if(ccv::debug) std::cerr << "size=" << wStep << "\n";
//   double mse;
//   int disparity,diffV,last_disp=0, llast_disp=0;
//   uchar* rptr;
//   uchar* lptr;
//   uchar* dptr;
  
//   // dispI = cvCloneImage(right);
//   zapImg(dispI);
//   dispI = cvCreateImage( cvSize(right->width-MAXD,
// 				right->height), 
// 			 IPL_DEPTH_8U, 1);
//   cvSetZero( dispI );

//   int n = left->nChannels;

//   for(int x=1;x<(width-MAXD);x++) {
//     for(int y=1;y<(height-1);y++) {
      
//       mse = 1e13; disparity = 0;
//       dptr = ccv::pixel_u(dispI,x,y);
//       rptr = ccv::pixel_u(right,x,y);
//       lptr = ccv::pixel_u(left,x,y);

//       for(int offset=0;offset<MAXD;offset++) {
	
// 	if(!square) {
// 	  diffV = 
// 	    abs( (rptr-wStep-n)[0] - (lptr-wStep-n)[0] ) +
// 	    abs( (rptr-wStep+0)[0] - (lptr-wStep+0)[0] ) +
// 	    abs( (rptr-wStep+n)[0] - (lptr-wStep+n)[0] ) +
// 	    abs( (rptr      -n)[0] - (lptr      -n)[0] ) +
// 	    abs( (rptr      +0)[0] - (lptr      +0)[0] ) +
// 	    abs( (rptr      +n)[0] - (lptr      +n)[0] ) +
// 	    abs( (rptr+wStep-n)[0] - (lptr+wStep-n)[0] ) +
// 	    abs( (rptr+wStep+0)[0] - (lptr+wStep+0)[0] ) +
// 	    abs( (rptr+wStep+n)[0] - (lptr+wStep+n)[0] );
	  
// 	  if(left->nChannels > 1 && color) {
// 	    diffV += 
// 	      abs( (rptr-wStep-n)[1] - (lptr-wStep-n)[1] ) +
// 	      abs( (rptr-wStep+0)[1] - (lptr-wStep+0)[1] ) +
// 	      abs( (rptr-wStep+n)[1] - (lptr-wStep+n)[1] ) +
// 	      abs( (rptr      -n)[1] - (lptr      -n)[1] ) +
// 	      abs( (rptr      +0)[1] - (lptr      +0)[1] ) +
// 	      abs( (rptr      +n)[1] - (lptr      +n)[1] ) +
// 	      abs( (rptr+wStep-n)[1] - (lptr+wStep-n)[1] ) +
// 	      abs( (rptr+wStep+0)[1] - (lptr+wStep+0)[1] ) +
// 	      abs( (rptr+wStep+n)[1] - (lptr+wStep+n)[1] );
	    
// 	    diffV += 
// 	      abs( (rptr-wStep-n)[2] - (lptr-wStep-n)[2] ) +
// 	      abs( (rptr-wStep+0)[2] - (lptr-wStep+0)[2] ) +
// 	      abs( (rptr-wStep+n)[2] - (lptr-wStep+n)[2] ) +
// 	      abs( (rptr      -n)[2] - (lptr      -n)[2] ) +
// 	      abs( (rptr      +0)[2] - (lptr      +0)[2] ) +
// 	      abs( (rptr      +n)[2] - (lptr      +n)[2] ) +
// 	      abs( (rptr+wStep-n)[2] - (lptr+wStep-n)[2] ) +
// 	      abs( (rptr+wStep+0)[2] - (lptr+wStep+0)[2] ) +
// 	      abs( (rptr+wStep+n)[2] - (lptr+wStep+n)[2] );
// 	  }
// 	}
	
// 	else {
// 	  diffV = 
// 	    pow( (double)(rptr-wStep-n)[0] - (lptr-wStep-n)[0] ,2) +
// 	    pow( (double)(rptr-wStep+0)[0] - (lptr-wStep+0)[0] ,2) +
// 	    pow( (double)(rptr-wStep+n)[0] - (lptr-wStep+n)[0] ,2) +
// 	    pow( (double)(rptr      -n)[0] - (lptr      -n)[0] ,2) +
// 	    pow( (double)(rptr      +0)[0] - (lptr      +0)[0] ,2) +
// 	    pow( (double)(rptr      +n)[0] - (lptr      +n)[0] ,2) +
// 	    pow( (double)(rptr+wStep-n)[0] - (lptr+wStep-n)[0] ,2) +
// 	    pow( (double)(rptr+wStep+0)[0] - (lptr+wStep+0)[0] ,2) +
// 	    pow( (double)(rptr+wStep+n)[0] - (lptr+wStep+n)[0] ,2);
	  
// 	  if(left->nChannels > 1 && color) {
// 	    diffV += 
// 	      pow( (double)(rptr-wStep-n)[1] - (lptr-wStep-n)[1] ,2) +
// 	      pow( (double)(rptr-wStep+0)[1] - (lptr-wStep+0)[1] ,2) +
// 	      pow( (double)(rptr-wStep+n)[1] - (lptr-wStep+n)[1] ,2) +
// 	      pow( (double)(rptr      -n)[1] - (lptr      -n)[1] ,2) +
// 	      pow( (double)(rptr      +0)[1] - (lptr      +0)[1] ,2) +
// 	      pow( (double)(rptr      +n)[1] - (lptr      +n)[1] ,2) +
// 	      pow( (double)(rptr+wStep-n)[1] - (lptr+wStep-n)[1] ,2) +
// 	      pow( (double)(rptr+wStep+0)[1] - (lptr+wStep+0)[1] ,2) +
// 	      pow( (double)(rptr+wStep+n)[1] - (lptr+wStep+n)[1] ,2);
	    
// 	    diffV += 
// 	      pow( (double)(rptr-wStep-n)[2] - (lptr-wStep-n)[2] ,2) +
// 	      pow( (double)(rptr-wStep+0)[2] - (lptr-wStep+0)[2] ,2) +
// 	      pow( (double)(rptr-wStep+n)[2] - (lptr-wStep+n)[2] ,2) +
// 	      pow( (double)(rptr      -n)[2] - (lptr      -n)[2] ,2) +
// 	      pow( (double)(rptr      +0)[2] - (lptr      +0)[2] ,2) +
// 	      pow( (double)(rptr      +n)[2] - (lptr      +n)[2] ,2) +
// 	      pow( (double)(rptr+wStep-n)[2] - (lptr+wStep-n)[2] ,2) +
// 	      pow( (double)(rptr+wStep+0)[2] - (lptr+wStep+0)[2] ,2) +
// 	      pow( (double)(rptr+wStep+n)[2] - (lptr+wStep+n)[2] ,2);
// 	  }
// 	}

// 	lptr+=left->nChannels;
// 	iterations++;
// 	if(diffV<mse) { mse = diffV; disparity = offset; }
//       }
//       // continuity constraint
//       //       if(disparity<last_disp-1) disparity=last_disp-1;     
//       //       else if(disparity>last_disp+1) disparity=last_disp+1;
      
// //       if(disparity==MAXD) disparity=last_disp;
// //       if(abs(disparity-llast_disp)<3) {
// // 	uchar val = static_cast<uchar>((static_cast<float>(disparity)/MAXD)*256);
// // 	(dptr-3)[0] = val;
// // 	(dptr-3)[1] = val;
// // 	(dptr-3)[2] = val;
// // 	last_disp=disparity;
// //       }

// //       llast_disp=last_disp;
// //       last_disp=disparity;

//       val = static_cast<uchar>((static_cast<float>(disparity)/MAXD)*256);
      
//       (dptr)[0] = val ;    // blue
//       (dptr)[1] = val;     // green
//       (dptr)[2] = val;     // red

//     }
//   }
// }

bool
BlockMatch::calculateDisparityFast()
{
  int nc;
  if(!color) nc = 1;
  else nc = left->nChannels;

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
      rptr = ccv::pixel_u(right,x-1,y-1);
      lptr = ccv::pixel_u(left,x-1,y-1);
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
  dispI = cvCreateImage( cvSize(right->width,
				right->height), 
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
  dispI = cvCreateImage( cvSize(right->width,
				right->height), 
			 IPL_DEPTH_8U, 1);
  cvSetZero( dispI );

  IplImage* tmpL = cvCreateImage( cvSize(left->width+bs-1+MAXD,
					 left->height+bs-1),
				  left->depth, left->nChannels); 

  IplImage* tmpR = cvCreateImage( cvSize(right->width+bs-1+MAXD,
					 right->height+bs-1),
				  right->depth, right->nChannels); 
  
  cvSetZero( tmpL );
  cvSetZero( tmpR );

  int bs2 = bs/2;

  for(int x=0; x<left->width; x++)
    for(int y=0; y<left->height; y++) 
      for(int c=0; c<left->nChannels; c++) {
	ccv::pixel_u(tmpL, x+bs2, y+bs2 )[c] = 
	  ccv::pixel_u(left, x, y )[c];
	ccv::pixel_u(tmpR, x+bs2, y+bs2 )[c] = 
	  ccv::pixel_u(right, x, y )[c];
      }

//   cvSaveImage("lll.bmp",tmpL);
//   cvSaveImage("rrr.bmp",tmpR);
  
  if(ccv::debug) std::cerr << "tmp images set up\n";

  for(int x=0; x<left->width; x++) {
    for(int y=0; y<left->height; y++) {
      
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

