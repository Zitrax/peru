/*************************************************

 Purpose:
   This file contains the ccv namespace which
   contains useful constructions used by Peru

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: ccv.h,v 1.4 2003/09/25 23:49:27 cygnus78 Exp $

*************************************************/

#ifndef CCV_H
#define CCV_H

#include <qstring.h>
#include <qimage.h>

#include <opencv/cv.h>
#include <iostream>

#define zapMat(x) if(x){if(ccv::debug) std::cerr << "Zapping\n"; cvReleaseMat(&x); x=0;}
#define zapImg(x) if(x){cvReleaseImage(&x); x=0;}
#define zap(x) if(x){delete(x); x=0;}
#define zapArr(x) if(x){delete [] x; x=0;}

namespace ccv{
  
  extern int debug;

  enum{ERROR, NOTUSED};

  extern bool ERRFLAG;

  struct error{
    QString msg;
    error(QString s) : msg(s){}
  };

  uchar* pixel_u(IplImage* image, //!< More convenient way of accessing pixels
			int x, int y);

  float* pixel_f(IplImage* image, 
			int x, int y);

  //! Stretches the contrast of an image (min->0 max->255)
  void contrastStretch(IplImage* img); 

  QImage* iplImageToQImage(IplImage* ipl_img,
			   bool special=false,
			   int color = 1 );

  IplImage* qImageToIplImage(QImage* qimg);
}

std::ostream& operator<<(std::ostream& os, const CvMat& m);

#endif
