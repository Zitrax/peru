/*************************************************

 Purpose:
   This filter removes gradients in images
   Purpose in peruapplication is to eliminate
   the gradients left by the blockmatching 
   algorithms.

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info

 Version:
   $Id: GradientRemoval.cpp,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

*************************************************/

#include "GradientRemoval.h"

GradientRemoval::GradientRemoval() : Filter() {}

void
GradientRemoval::apply(IplImage* image)
{
  if(ccv::debug) std::cerr << "Applying gradient removal filter\n";

  if(ccv::debug) std::cerr << "Image: " << image->width << "," 
			   << image->height << "\n";

  uchar triple[3];
  triple[0] = 0;
  triple[1] = 0;
  triple[2] = 0;

  int tripleindex = 0;
  uchar* last=0;

  for(int y=0; y<image->height; y++)
    for(int x=0; x<image->width; x++) {
      triple[tripleindex++] = *ccv::pixel_u(image, x, y);
      tripleindex %= 3;
      if( triple[0] != triple[1] )
	if( triple[0] != triple[2] )
	  if( triple[1] != triple[2] )
	    if(last) 
	      *last = 0;
	    
      last = ccv::pixel_u(image, x, y);
    }
}
