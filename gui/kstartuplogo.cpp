/* Original file from KDE (www.kde.org)
   Slightly modified by Daniel Bengtsson 030830 */

#include "kstartuplogo.h"

#include <ctime>
#include <cstdlib>

#include <qdir.h>

#include "../src/ccv.h"

namespace PeruGlobals{
  extern QString execLocation;
}

KStartupLogo::KStartupLogo(QWidget * parent, const char *name) 
  : QWidget(parent,name, WStyle_NoBorderEx | WStyle_Customize | 
	    WDestructiveClose ), 
    m_bReadyToHide( false ),
    m_have_image( false )
{

  QPixmap pm;

  QDir image_dir( PeruGlobals::execLocation + "images");
  const QStringList images = image_dir.entryList("*.bmp *.jpg *.png");

  if( images.count() ) {
    
    m_have_image = true;

    if(ccv::debug) ccv::print( images );
    
    // Create random number
    srand(static_cast<unsigned>(time(0)));
    
    pm.load( PeruGlobals::execLocation + "images/" + images[rand()%images.size()] );
    
    setBackgroundPixmap(pm);
    
    // We don't have any geometry yet so we just have to go with the cursor
    QRect desk = QApplication::desktop()->screenGeometry
      (QApplication::desktop()->screenNumber(QCursor::pos()));
    
    setGeometry(desk.center().x()-pm.width()/2, 
		desk.center().y()-pm.height()/2, pm.width(),pm.height());
  }
}

void KStartupLogo::mousePressEvent( QMouseEvent*)
{
  if (m_bReadyToHide) hide();
}
