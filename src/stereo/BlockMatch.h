/*************************************************

 Purpose:
   Subclass of Stereo
   Implements a blockmatching stereo algorithm

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: BlockMatch.h,v 1.1.1.1 2003/09/04 21:11:25 cygnus78 Exp $

*************************************************/

#ifndef BLOCKMATCH_H
#define BLOCKMATCH_H

#include "stereo.h"

class BlockMatch : public Stereo
{
  bool color;
  bool square;
  int bs;

  int windowDiff(uchar* l, uchar* r, int wstep, int n, int nc);

 public:
  BlockMatch(int argc, char** argv);
  BlockMatch(int argc, char** argv, bool c, int bls=3, bool f=false, bool sq=false);
  bool calculateDisparity();
  bool calculateDisparityFast();
  bool fast;
};

#endif
