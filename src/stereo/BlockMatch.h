// -*- mode: C++; -*-
/*************************************************

 Purpose:
   Subclass of Stereo
   Implements a blockmatching stereo algorithm

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: BlockMatch.h,v 1.2 2003/09/05 12:15:12 cygnus78 Exp $

*************************************************/

#ifndef BLOCKMATCH_H
#define BLOCKMATCH_H

#include "stereo.h"

class BlockMatch : public Stereo
{
  bool color;
  bool square;
  int bs;
  int MAXD;

  int windowDiff(uchar* l, uchar* r, int wstep, int n, int nc);

 public:
  BlockMatch(string left, string right, string out);
  BlockMatch(string left, string right, string out, 
	     bool c, int maxd=15, int bls=3, bool f=false, bool sq=false);
  bool calculateDisparity();
  bool calculateDisparityFast();
  bool fast;
};

#endif
