// -*- mode: C++; -*-
/*************************************************

 Purpose:
   Hierarchical block stereo algorithm

 Author:
   Daniel Bengtsson 2002, daniel@bengtssons.info

 Version:
   $Id: PyramidBlock.h,v 1.6 2007/03/22 23:42:39 cygnus78 Exp $

*************************************************/

#ifndef PYRAMIDBLOCK_H
#define PYRAMIDBLOCK_H

#include "stereo.h"

using namespace std;

class PyramidBlock : public Stereo
{
  int levels;                        //!< Number of levels to downsample
  int* tolerance;                    //!< Disp tolerance between levels
  bool color;
  bool error;                        //!< Used to tell destructor about fails
  int MAXD;

  IplImage** pyramid_left;
  IplImage** pyramid_right;

  int*** pyr_disp;                   //!< Disparities found in different levels

 public:
  PyramidBlock(string left, string right, string out,
	       int maxd,
	       int levels,
	       int tol,
	       bool c);
  ~PyramidBlock();

  /// Forbid copying
  PyramidBlock(const PyramidBlock&);
  /// Forbid assignment
  const PyramidBlock& operator=(const PyramidBlock&);

 protected:
  bool calculateDisparity();
  void blockmatch(int level);
};

#endif
