/*************************************************

 Purpose:
   Display and do simple manipulations on images

 Author:
   Daniel Bengtsson, danielbe@ifi.uio.no

 Version:
   $Id: ImageWidget.h,v 1.5 2004/10/02 11:10:02 cygnus78 Exp $
*************************************************/

#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <qwidget.h>
#include <qimage.h>
#include <qpainter.h>
#include <qevent.h>
#include <qcolor.h>

#include "../src/ccv.h"

class ImageWidget : public QWidget
{
  Q_OBJECT
public:
  ImageWidget( QWidget *parent=0,
	       const char *name=0,
	       bool scale=false);
  ~ImageWidget();

  QImage getImage();
  void setImage(QImage img);
  void smoothScaleImage(); 
  float getXScale();                  //!< Original/Scale
  float getYScale();                  //!< Original/Scale
  int getWidth();
  int getHeight();
  int valueAt(int x, int y);
  void colormap(bool map);            //!< Applies colormap
  void displayCurrentImage();     
  bool saveImage(const QString& filename, 
		 const char* format);

public slots:
  void displayImage(QImage& im);
  void setScaled(bool scale);         //!< Should the image be scaled ?

signals:
  void sendMousePressed(QMouseEvent* e);
  void sendMouseReleased(QMouseEvent* e);
  
protected:
  void paintEvent(QPaintEvent *e);
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  QSize sizeHint() const;
  bool eventFilter( QObject* target, QEvent* event );

private:
  QImage oImage;                      //!< Original image
  QImage sImage;                      //!< Scaled image
  bool scaled;                        //!< Should the image be scaled ?

};

#endif


