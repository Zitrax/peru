/*************************************************

 Purpose:
   Hierarchical block stereo algorithm

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: PyramidBlock.h,v 1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#include "stereo.h"

class PyramidBlock : public Stereo
{
  int levels;                        //!< Number of levels to downsample
  int* tolerance;                    //!< Disp tolerance between levels
  bool color;
  bool error;                        //!< Used to tell destructor about fails

  IplImage** pyramid_left;
  IplImage** pyramid_right;

  int*** pyr_disp;                   //!< Disparities found in different levels

 public:
  PyramidBlock(int argc, 
	       char** argv, 
	       int levels,
	       int tol,
	       bool c);
  ~PyramidBlock();
 protected:
  bool calculateDisparity();
  void blockmatch(int level);
};
