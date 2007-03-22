/* Original file from KDE (www.kde.org)
   Slightly modified by Daniel Bengtsson 030830 */

#ifndef KSTARTUPLOGO_H
#define KSTARTUPLOGO_H

#include "inc/kstartuplogo_externals.h"

/**
  *@author Kai Heitkamp
  */

class KStartupLogo : public QWidget  {
   Q_OBJECT

public:
   KStartupLogo(QWidget *parent=0, const char *name=0);
   ~KStartupLogo(){}
   
   void setHideEnabled(bool bEnabled) { m_bReadyToHide = bEnabled; };
   
   /** Returns true if we have an image to show */
   bool haveImage() const { return m_have_image; }

protected:
   virtual void mousePressEvent( QMouseEvent*);
   bool m_bReadyToHide;
   bool m_have_image;
};

#endif
