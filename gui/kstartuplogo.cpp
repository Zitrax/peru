/* Original file from KDE (www.kde.org)
   Slightly modified by Daniel Bengtsson 030830 */

#include "kstartuplogo.h"

KStartupLogo::KStartupLogo(QWidget * parent, const char *name) 
  : QWidget(parent,name, WStyle_NoBorderEx | WStyle_Customize | 
	    WDestructiveClose ), m_bReadyToHide( false )
{
  QPixmap pm;
  pm.load("images/krgmachuh.bmp");
  setBackgroundPixmap(pm);
  // We don't have any geometry yet so we just have to go with the cursor
  QRect desk = QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(QCursor::pos()));
  setGeometry(desk.center().x()-pm.width()/2, desk.center().y()-pm.height()/2, pm.width(),pm.height());
}

KStartupLogo::~KStartupLogo(){
}

void KStartupLogo::mousePressEvent( QMouseEvent*){
  if (m_bReadyToHide) hide();
}
