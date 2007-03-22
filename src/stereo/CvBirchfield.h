// -*- mode: C++; -*-
/*************************************************

 Purpose:
   This class uses the birchfield stereo algorithm
   that comes with OpenCv 0.9.4 beta3

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: CvBirchfield.h,v 1.5 2007/03/22 23:42:39 cygnus78 Exp $

*************************************************/

#ifndef CVBIRCHFIELD_H
#define CVBIRCHFIELD_H

#include "stereo.h"

#include "inc/CvBirchfield_externals.h"

class CvBirchfield : public Stereo
{
private:
  int MAXD;
public:
  CvBirchfield(string left, string right, string out, int maxd);  
  bool calculateDisparity();            //!< Starts the algorithm
};

#endif
