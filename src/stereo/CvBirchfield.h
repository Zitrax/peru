// -*- mode: C++; -*-
/*************************************************

 Purpose:
   This class uses the birchfield stereo algorithm
   that comes with OpenCv 0.9.4 beta3

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: CvBirchfield.h,v 1.4 2005/06/22 23:14:23 cygnus78 Exp $

*************************************************/

#ifndef CVBIRCHFIELD_H
#define CVBIRCHFIELD_H

#include "stereo.h"
#include <opencv/cvaux.h>

class CvBirchfield : public Stereo
{
private:
  int MAXD;
public:
  CvBirchfield(string left, string right, string out, int maxd);  
  bool calculateDisparity();            //!< Starts the algorithm
};

#endif
