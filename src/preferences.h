/*************************************************

 Purpose:
   This class will handle settings in between
   sessions.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: preferences.h,v 1.3 2005/07/01 22:30:21 cygnus78 Exp $

*************************************************/

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <qstring.h>

class Peru;

class Preferences
{

public:

  Preferences(Peru* peru);
  ~Preferences(){}
 
  void readSettings() const;
  void writeSettings();

  QString getCalibrationPath() const;

private:

  Peru* m_peru;

};

#endif
