/*************************************************

 Purpose:
   This class will handle settings in between
   sessions.

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: preferences.h,v 1.1 2004/10/02 11:15:11 cygnus78 Exp $

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
