/*************************************************

 Purpose:
   This file contains the ccv namespace which
   contains useful constructions used by Peru

 Author:
   Daniel Bengtsson 2002, daniel@bengtssons.info

 Version:
   $Id: ccv.h,v 1.11 2007/03/22 23:42:39 cygnus78 Exp $

*************************************************/

#ifndef CCV_H
#define CCV_H

#include "inc/ccv_externals.h"

#define zapMat(x) if(x){if(ccv::debug) std::cerr << "Zapping\n"; cvReleaseMat(&x); x=0;}
#define zapImg(x) if(x){cvReleaseImage(&x); x=0;}
#define zap(x) if(x){delete(x); x=0;}
#define zapArr(x) if(x){delete [] x; x=0;}

#define XOR(a,b) (!(a) != !(b))
#define XNOR(a,b) ( (!a&&!b) || (a&&b) )

namespace ccv{
  
  extern int debug;

  extern QString ERROR_MESSAGE;

  enum{ERROR, NOTUSED};

  /// Used to abort all calculations
  extern bool ERRFLAG;

  extern bool ABORTFLAG;

  void resetError();

  struct error{
    QString msg;
    error(QString s) : msg(s){}
  };

  //! More convenient way of accessing pixels
  uchar* pixel_u(IplImage* image, 
			int x, int y);

  float* pixel_f(IplImage* image, 
			int x, int y);

  //! Stretches the contrast of an image (min->0 max->255)
  void contrastStretch(IplImage* img); 

  QImage* iplImageToQImage(IplImage* ipl_img,
			   bool special=false,
			   int color = 1 );

  IplImage* qImageToIplImage(QImage* qimg);

  void print( const QStringList& str_list );

  //! Not exception safe however
  struct delete_object
  {
      template <typename T> void operator()(const T* ptr) const
          { delete ptr; }
  };

  inline int round(double x)
  {
      return int(x > 0.0 ? x + 0.5 : x - 0.5);
  }

}

std::ostream& operator<<(std::ostream& os, const CvMat& m);

#endif
