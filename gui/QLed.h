// -*- mode: C++; -*-
/*************************************************

 Purpose:
   This class makes a small widget that is used
   as a green/red (on/off) led/diode.

 Author:
   Daniel Bengtsson 2003, danielbe@ifi.uio.no
   Based on a post by Neil Zanella in the
   Qt-interest Archive.

 Version:
   $Id: QLed.h,v 1.1 2003/09/22 19:40:53 cygnus78 Exp $

*************************************************/

#ifndef QLED_H
#define QLED_H

#include <qwidget.h>
#include <qpalette.h>
#include <qcolor.h>
#include <qpainter.h>

class QLed: public QWidget {
    Q_OBJECT
  public:
    QLed(QWidget *parent = 0, const char *name = 0);
  public slots:
    void switchQLed();
    void setLed(bool on);
  protected:
    void paintEvent(QPaintEvent *);
  private:
    bool lit;
    const int width, height;
};

#endif
