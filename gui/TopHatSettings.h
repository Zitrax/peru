// -*- mode: C++; -*-
/*************************************************

 Purpose:
   This is small dialog for tuning the settings
   of the TopHat transformation.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: TopHatSettings.h,v 1.1 2003/09/07 19:59:54 cygnus78 Exp $

*************************************************/

#ifndef TOPHATSETTINGS_H
#define TOPHATSETTINGS_H

#include "ccv.h"
#include "TopHatSettingsBase.h"
#include "Peru.h"
#include "CCOCV.h"

#include <qwidget.h>
#include <qlcdnumber.h>

class CCOCV;
class Peru;

class TopHatSettings : public TopHatSettingsBase
{
  Q_OBJECT
public:
  TopHatSettings( CCOCV* c,
		  Peru* p,
		  QWidget* parent = 0, 
		  const char* name = 0,
		  WFlags fl = 0 );
  ~TopHatSettings();
 
public slots:
  void calculate();

private:
  CCOCV* calib;
  Peru* gui;

};


#endif
