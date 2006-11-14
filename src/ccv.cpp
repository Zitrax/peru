/*************************************************

 Purpose:
   This file contains the ccv namespace which
   contains useful constructions and methods

 Author:
   Daniel Bengtsson 2002, daniel@bengtssons.info

 Version:
   $Id: ccv.cpp,v 1.9 2006/11/14 22:05:24 cygnus78 Exp $

*************************************************/

#include "ccv.h"

namespace ccv {
  
  int debug = 0;
  bool ERRFLAG = false;
  bool ABORTFLAG = false;
  QString ERROR_MESSAGE = QString::null;
  
  void resetError()
  {
    ERROR_MESSAGE = QString::null;
    ERRFLAG = false;
  }

  uchar*
  pixel_u(IplImage* image,int x, int y)
  {
    return (uchar*)(image->imageData + y*image->widthStep + x*image->nChannels);
  }
  
  float*
  pixel_f(IplImage* image,int x, int y)
  {
    return (float*)(image->imageData + y*image->widthStep + x*image->nChannels);
  }
  
  void
  contrastStretch(IplImage* img)
  {
    if(ccv::debug) std::cerr << "ccv::contrastStretch\n";
    //First find min and max value
    double* min = new double;
    double* max = new double;
    CvPoint* notused = new CvPoint;
    
    cvMinMaxLoc(img, min, max, notused, notused );
    
    float diff = *max - *min;
    
    if(ccv::debug) std::cerr << "(Min,Max,Diff) = " 
			     << *min << "," << *max << "," << diff << "\n";
    
    for(int x=0; x<img->width; x++)
      for(int y=0; y<img->height; y++)
	*ccv::pixel_u(img,x,y) = 
	  static_cast<uchar>(((*ccv::pixel_u(img,x,y) - *min)/diff)*255);
    
    delete min;
    delete max;
    delete notused;
  }
 
  /*!
   * The special flag is specially for disparity point clouds
   * and should prefferably be defaulted to false as is the case
   * in Peru.h. This method should be splitted in two..!
   */
  QImage*
  iplImageToQImage(IplImage* ipl_img, bool special, int color) 
  {
    if(ccv::debug) std::cerr << "Converting\n";
    if(ccv::debug) std::cerr << "nChannels = " << ipl_img->nChannels << "\n";
    if(ccv::debug) std::cerr << "depth = " << ipl_img->depth << "\n";
    if(ccv::debug) std::cerr << "special = " << special << "\n";
    
    QImage* q = new QImage(ipl_img->width,ipl_img->height,32);
    
    int x; int xxx;
    int y;
    char *data = ipl_img->imageData;
    if(!special && ipl_img->nChannels==3) {
      for( y = 0; y < ipl_img->height; y++, data += ipl_img->widthStep )
	for( x = 0; x < ipl_img->width; x++ )
	  {
	    xxx = 3*x;
	    uint *p = (uint*)q->scanLine(y) + x;
	    *p = qRgb(data[xxx + 2],
		      data[xxx + 1],
		      data[xxx + 0]);
	    
	  }
      return q;
    }
    if(!special && ipl_img->nChannels==1) {
      for( y = 0; y < ipl_img->height; y++, data += ipl_img->widthStep )
	for( x = 0; x < ipl_img->width; x++ )
	  {
	    uint *p = (uint*)q->scanLine(y) + x;
	    int color = qRgb(data[x],
			     data[x],
			     data[x]);
	    *p = color;
	    //if(ccv::debug) std::cerr << "data[x] = " << (uint)data[x] << "\n";  
	  }
      return q;
    }
    else if(special)
      for( y = 0; y < ipl_img->height; y++, data += ipl_img->widthStep )
	for( x = 0; x < ipl_img->width; x++ )
	  {
	    uint *p = (uint*)q->scanLine(y) + x;
	    if(data[x]!=0)
	      switch(color) {
	      case 1:
		*p = qRgb(data[x],
			  255-data[x],
			  255-data[x]);
		break;
	      case 2:
		*p = qRgb(data[x],
			  0,
			  255-data[x]);
		break;
	      case 3:
		*p = qRgb(0,
			  255-data[x],
			  data[x]);
		break;
	      }
	    else
	      *p = qRgb(0,
			0,
			0);
	  }
    else
      if(ccv::debug) std::cerr << "ERROR - No conversion ipltoqimage available\n";
    
    return q;
  } 
  
  //! Only 1 channel currently
  IplImage*
  qImageToIplImage(QImage* qimg)
  {
    IplImage* iplimg = cvCreateImage
      ( cvSize(qimg->width(),qimg->height()), IPL_DEPTH_8U, 1);
    //qimg->pixel(x,y)
    for(int x=0; x<iplimg->width; x++)
      for(int y=0; y<iplimg->height; y++) {
	QRgb c = qimg->pixel(x,y);
	*(iplimg->imageData + y*iplimg->widthStep + x*iplimg->nChannels) 
	  = qRed(c);
      }

    return iplimg;
  }
  
}

std::ostream& operator<<(std::ostream& os, const CvMat& m)
{
    os << "PRINTING MATRIX (rows=" << m.rows << ",cols=" << m.cols << ")\n";

    for(int r=0; r<m.rows; r++)
    {
        for(int c=0; c<m.cols; c++) 
            os << CV_MAT_ELEM( m, float, r, c ) << "\t";
        os << "\n";
    }
    return os;
}

void ccv::print( const QStringList& str_list )
{
  QStringList::ConstIterator it  = str_list.begin();
  QStringList::ConstIterator end = str_list.end();

  for( ; it!=end; ++it )
    std::cout << *it << std::endl;
}

