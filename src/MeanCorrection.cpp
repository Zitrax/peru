/*************************************************

 Purpose:
   This class implements the MeanCorrection noise 
   removal filter

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: MeanCorrection.cpp,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include "MeanCorrection.h"

MeanCorrection::MeanCorrection(IplImage** i1,
			       IplImage** i2) : 
  Filter(), image1(i1), image2(i2) 
{
  flip = false;
  if(ccv::debug) std::cerr << "I: " << image2 << "," << i2 << "\n";
}

void
MeanCorrection::apply(IplImage* image)
{
  if(ccv::debug) std::cerr << "Applying MeanCorrection filter\n";

  if(!flip) {
    int add = cvRound(findMean(*image1)-findMean(*image2));
    if(ccv::debug) std::cout << "Add: " << add << "\n";
    addC(*image2,add);
  }
  
  // Evil hack to not do this filter double
  if(flip) flip=false;
  else flip=true;
}

double
MeanCorrection::findMean(IplImage* img)
{
  int sum=0;
  int nop = img->height*img->width;

  if(img->nChannels != 3) 
    for(int i=0; i<nop; i++)
      sum += static_cast<uchar>(img->imageData[i]);

  else
    for(int i=0; i<3*nop; i+=3) {
      sum += static_cast<uchar>(img->imageData[i+0]);
      sum += static_cast<uchar>(img->imageData[i+1]);
      sum += static_cast<uchar>(img->imageData[i+2]);
    }

  double mean;

  if(img->nChannels != 3) mean = static_cast<double>(sum)/nop;
  else mean = static_cast<double>(sum)/(3*nop);

  return mean;
}

void
MeanCorrection::addC(IplImage* img, int value)
{
  int nop = img->height*img->width;
  
  if(img->depth==IPL_DEPTH_8U) {
    for(int i=0; i<(img->nChannels*nop); i+=img->nChannels) 
      for(int c=0; c<img->nChannels; c++) {
	uchar* p = (uchar*)(&img->imageData[i+c]);
	if((*p + value)>255) *p=255;
	else if((*p + value)<0) *p=0;
	else *p += value;
      }
  }
  else
    if(ccv::debug) 
      std::cerr << "ERROR - Stereo::add does only support IPL_DEPTH_8U images\n";
   
  if(ccv::debug) std::cerr << "I: " << img << "\n";
  if(ccv::debug) cvSaveImage("addc.bmp",img);
}

