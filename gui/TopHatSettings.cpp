/*************************************************

 Purpose:
   This is small dialog for tuning the settings
   of the TopHat transformation.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: TopHatSettings.cpp,v 1.3 2004/05/20 22:47:51 cygnus78 Exp $

*************************************************/

#include "TopHatSettings.h"

TopHatSettings::TopHatSettings( CCOCV* c,
				Peru* p,
				QWidget* parent, 
				const char* name,
				WFlags fl) 
  : TopHatSettingsBase(parent, name, fl), gui(p), calib(c), trialImage(0) {}

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

    if( trialImage ) {
      delete trialImage;
      trialImage = 0;
    }

    trialImage = ccv::iplImageToQImage(image);

    zapImg(image);

    gui->imageOpen(*trialImage);

  }
  else if (realTimeCB->isChecked() && calib->getNumberOfFilesInList() < 1 ) { 
    if(ccv::debug) std::cerr << "ERROR - No file(s)\n"; 
  }
    
}
