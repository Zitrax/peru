// -*- mode: C++; -*-
/*************************************************

 Purpose:
   This class makes a small widget that is used
   as a green/red (on/off) led/diode.

 Author:
   Daniel Bengtsson 2003-2005, daniel@bengtssons.info
   Based on a post by Neil Zanella in the
   Qt-interest Archive.

 Version:
   $Id: QLed.h,v 1.2 2005/06/22 23:14:24 cygnus78 Exp $

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
