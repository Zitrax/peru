/*************************************************

 Purpose:
   This class implements Histogram equalization

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no

 Version:
   $Id: HistoEq.cpp,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include "HistoEq.h"

HistoEq::HistoEq() : Filter() {}

void
HistoEq::apply(IplImage* image)
{
  if(ccv::debug) std::cerr << "Applying HistoEq filter\n";

  histogramEqualize(image);
  
}

void
HistoEq::histogramEqualize(IplImage* img)
{

  if(ccv::debug) std::cerr << "WARNING - HistoEq::histogramEqualize does"
			   << " nothing at the moment... (needs remake)\n";

//   IplImage* tmp;

//   tmp = cvCloneImage( img );        
	
//   // To equalize the histogram we first need a lookup table
//   int range = 256;
//   IplLUT** plut = new IplLUT*[3];

//   // Yiiiikes you scream, why not a loop ?
//   // Well I thought about that too but...  fix it if you are pedantic ;)
//   // If you want to use only one channel that is probably a good idea
//   IplLUT lut = { range+1, NULL,NULL,NULL, IPL_LUT_INTER };

//   lut.key    = new int[range+1];
//   lut.value  = new int[range];
//   lut.factor = new int[range];
      
//   for( int i=0; i<=range; i++) 
//     lut.key[i] = i;
	
//   IplLUT lut2 = { range+1, NULL,NULL,NULL, IPL_LUT_INTER };

//   lut2.key    = new int[range+1];
//   lut2.value  = new int[range];
//   lut2.factor = new int[range];
      
//   for( int i=0; i<=range; i++) 
//     lut2.key[i] = i;

//   IplLUT lut3 = { range+1, NULL,NULL,NULL, IPL_LUT_INTER };

//   lut3.key    = new int[range+1];
//   lut3.value  = new int[range];
//   lut3.factor = new int[range];
      
//   for( int i=0; i<=range; i++) 
//     lut3.key[i] = i;

//   plut[0] = &lut; plut[1] = &lut2; plut[2] = &lut3; 

//   iplComputeHisto( tmp, plut );

//   iplHistoEqualize( tmp, img , plut); 

//   cvvSaveImage("barkmannen.bmp",img);
	
//   delete [] lut.key;
//   delete [] lut.value;
//   delete [] lut.factor;
//   delete [] lut2.key;
//   delete [] lut2.value;
//   delete [] lut2.factor;
//   delete [] lut3.key;
//   delete [] lut3.value;
//   delete [] lut3.factor;

//   delete [] plut;
//   cvReleaseImage(&tmp);
}
