/* Original file from KDE (www.kde.org)
   Slightly modified by Daniel Bengtsson 030830 */

#ifndef KSTARTUPLOGO_H
#define KSTARTUPLOGO_H

#include <qwidget.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qapplication.h>

/**
  *@author Kai Heitkamp
  */

class KStartupLogo : public QWidget  {
   Q_OBJECT

public:
KStartupLogo(QWidget *parent=0, const char *name=0);
~KStartupLogo();

void setHideEnabled(bool bEnabled) { m_bReadyToHide = bEnabled; };

protected:
  virtual void mousePressEvent( QMouseEvent*);
  bool m_bReadyToHide;
};

#endif
