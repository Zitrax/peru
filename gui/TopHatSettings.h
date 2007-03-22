// -*- mode: C++; -*-
/*************************************************

 Purpose:
   This is small dialog for tuning the settings
   of the TopHat transformation.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: TopHatSettings.h,v 1.6 2007/03/22 23:42:39 cygnus78 Exp $

*************************************************/

#ifndef TOPHATSETTINGS_H
#define TOPHATSETTINGS_H

#include "ccv.h"
#include "TopHatSettingsBase.h"
#include "CCOCV.h"
#include "Peru.h"

#include "inc/TopHatSettings_externals.h"

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

protected:
  void showEvent( QShowEvent* );

private:
  Peru* gui;
  CCOCV* calib;
  QImage *trialImage;

};


#endif
