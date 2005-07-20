/*************************************************

 Purpose:
   This class will handle settings in between
   sessions.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: preferences.h,v 1.4 2005/07/20 22:24:02 cygnus78 Exp $

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
  QString getStereoImagePath() const;

private:

  Peru* m_peru;

};

#endif
