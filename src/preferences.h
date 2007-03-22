/*************************************************

 Purpose:
   This class will handle settings in between
   sessions.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: preferences.h,v 1.5 2007/03/22 23:42:39 cygnus78 Exp $

*************************************************/

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "inc/preferences_externals.h"

class Peru;

class Preferences
{

public:

  Preferences(Peru* peru);
  ~Preferences(){}
 
  void readSettings() const;
  void writeSettings();

  QString getCalibrationPath() const;
  QString getStereoImagePath() const;

private:

  Peru* m_peru;

};

#endif
