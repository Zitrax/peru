/*************************************************

 Purpose:
   This is small dialog for tuning the settings
   of the TopHat transformation.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: TopHatSettings.cpp,v 1.2 2003/09/17 12:00:17 cygnus78 Exp $

*************************************************/

#include "TopHatSettings.h"

TopHatSettings::TopHatSettings( CCOCV* c,
				Peru* p,
				QWidget* parent, 
				const char* name,
				WFlags fl) 
  : TopHatSettingsBase(parent, name, fl), gui(p), calib(c) {}

TopHatSettings::~TopHatSettings(){}
 
void 
TopHatSettings::calculate()
{
  if(ccv::debug) std::cerr << "TopHatSettings::calculate\n";

  if(realTimeCB->isChecked() && calib->getNumberOfFilesInList() > 0) {
    
    gui->initializeCCOCV();

    int corners_found = 0;
    IplImage* image = calib->trialCalib(corners_found);
    foundLCD->display(corners_found);

    QImage* qimage = ccv::iplImageToQImage(image);

    zapImg(image);

    gui->imageOpen(*qimage);

  }
  else if (realTimeCB->isChecked() && calib->getNumberOfFilesInList() < 1 ) { 
    if(ccv::debug) std::cerr << "ERROR - No file(s)\n"; 
  }
    
}
