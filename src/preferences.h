/*************************************************

 Purpose:
   This class will handle settings in between
   sessions.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: preferences.h,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

*************************************************/

#ifndef PREFERENCES_H
#define PREFERENCES_H

class Peru;

class Preferences
{

public:

  Preferences(Peru* peru);
  ~Preferences(){}
 
  void readSettings() const;
  void writeSettings();

private:

  Peru* m_peru;

};

#endif
