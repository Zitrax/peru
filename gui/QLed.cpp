/*************************************************

 Purpose:
   This class makes a small widget that is used
   as a green/red (on/off) led/diode.

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no
   Based on a post by Neil Zanella in the
   Qt-interest Archive.

 Version:
   $Id: QLed.cpp,v 1.1 2003/09/22 19:40:54 cygnus78 Exp $

*************************************************/

#include "QLed.h"

QLed::QLed(QWidget *parent, const char *name):  QWidget(parent, name),
					      width(12), height(12) 
{
  setFixedSize(width, height);
  lit = false;
}

void 
QLed::paintEvent(QPaintEvent *) 
{
  lit ?  setPalette(QPalette(green)) : setPalette(QPalette(red));
  QPainter p(this);
  p.drawRect(0, 0, width, height);
}

void 
QLed::switchQLed() 
{
  lit = ! lit;
  repaint();
}

void
QLed::setLed(bool on)
{
  if( (!on && lit) || (on && !lit) )
    switchQLed();
}
