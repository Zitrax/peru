// -*- mode: C++; -*-
/*************************************************

 Purpose:
   Subclass of Stereo
   Implements a blockmatching stereo algorithm

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: BlockMatch.h,v 1.5 2004/09/24 21:36:42 cygnus78 Exp $

*************************************************/

#ifndef BLOCKMATCH_H
#define BLOCKMATCH_H

#include "stereo.h"
#include "../ccv.h"

using namespace std;

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
