// -*- mode: C++; -*-
/*************************************************

 Purpose:
   This class uses the birchfield stereo algorithm
   that comes with OpenCv 0.9.4 beta3

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: CvBirchfield.h,v 1.2 2003/09/05 12:15:12 cygnus78 Exp $

*************************************************/

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
