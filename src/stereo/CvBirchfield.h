/*************************************************

 Purpose:
   This class uses the birchfield stereo algorithm
   that comes with OpenCv 0.9.4 beta3

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: CvBirchfield.h,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include "stereo.h"
#include <opencv/cvaux.h>

class CvBirchfield : public Stereo
{
 public:
  CvBirchfield(int argc, char** argv);  
  bool calculateDisparity();            //!< Starts the algorithm
};
