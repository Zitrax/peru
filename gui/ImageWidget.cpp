/*************************************************

 Purpose:
   Display and do simple manipulations on images

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: ImageWidget.cpp,v 1.1 2003/09/04 21:11:23 cygnus78 Exp $

*************************************************/

#include "ImageWidget.h"
#include <qcheckbox.h>
#include <iostream>

ImageWidget::ImageWidget( QWidget *parent,
			  const char *name) : QWidget( parent, name) 
{
  scaled=false;
}

ImageWidget::~ImageWidget()
{
  delete &dImage;
  delete &oImage;
}

void 
ImageWidget::displayImage(QImage im)
{
  oImage=im;
  dImage=oImage;
  smoothScaleImage(scaled);

  // Check this later
  // bitBlt( this, 0, 0, &rendermap, 0, 0 ); 
}

void
ImageWidget::paintEvent( QPaintEvent *e)
{
  if(!oImage.isNull()){
    smoothScaleImage(scaled);
  }

}

void 
ImageWidget::smoothScaleImage(bool scale)
{
  if(!oImage.isNull()){                                     // Do not scale null-images
    if(scale){
      dImage=oImage.smoothScale(parentWidget()->size());
    }
    else { dImage=oImage; }
    QPainter qpainter( this );
    //qpainter.eraseRect( this->frameGeometry() );            // Should be modified, leaves edges
    qpainter.drawImage( 0, 0, dImage );
  }
}

QImage
ImageWidget::getImage()
{
  return oImage; // Maybe dImage ?
}

void
ImageWidget::setImage(QImage img)
{
  dImage=img;
  QPainter qpainter( this );
  qpainter.eraseRect( this->frameGeometry() );            // Should be modified, leaves edges
  qpainter.drawImage( 0, 0, dImage );

}

void
ImageWidget::setScaled(bool scale)
{
  scaled = scale;
  update();
}

void
ImageWidget::mousePressEvent(QMouseEvent* e)
{
  if(ccv::debug) std::cerr << "MousePressed at " << e->x() 
			   << "," << e->y() <<"\n";

  emit sendMousePressed(e);  
}

void
ImageWidget::mouseReleaseEvent(QMouseEvent* e)
{
  if(ccv::debug) std::cerr << "MouseReleased at " << e->x() 
			   << "," << e->y() <<"\n";
  
  emit sendMouseReleased(e);  
}

float
ImageWidget::getXScale()
{
  return oImage.width()/static_cast<float>(dImage.width());
}

float
ImageWidget::getYScale()
{
  return oImage.height()/static_cast<float>(dImage.height());
}

int
ImageWidget::getWidth()
{
  if(ccv::debug) std::cerr << "Width returned from ImageWidget is: " 
			   << parentWidget()->size().width() << "\n";
  return parentWidget()->size().width();
}

int
ImageWidget::getHeight()
{
  if(ccv::debug) std::cerr << "Height returned from ImageWidget is: " 
			   << parentWidget()->size().height() << "\n";
  return parentWidget()->size().height();
}

int
ImageWidget::valueAt(int x, int y)
{
  return qRed(oImage.pixel(x,y));
}

void 
ImageWidget::colormap(bool map)
{
  if(ccv::debug) std::cerr << "ImageWidget::colormap\n";
  if(ccv::debug) std::cerr << "Depth = " << oImage.depth() << "\n";
  if(map) {

    if(oImage.depth()==8) {
      for(int i=0; i<255; i++)
	oImage.setColor( i, qRgb(i,0,255-i) );
      
      for(int y=0; y<oImage.height()-1; y++)
	for(int x=0; x<oImage.width()-1; x++) {
	  *(oImage.scanLine(y) + x) = qRed(oImage.pixel(x,y));
	}
    }
    else {
      for(int y=0; y<oImage.height()-1; y++)
	for(int x=0; x<oImage.width()-1; x++) {
	  uint *p = (uint *)oImage.scanLine(y) + x;
	  *p = qRgb(qRed(oImage.pixel(x,y)),0,255-qRed(oImage.pixel(x,y)));
	}

    }
  }
}

void
ImageWidget::displayCurrentImage()
{
  smoothScaleImage(scaled); 
}

void
ImageWidget::saveImage()
{
  oImage.save(QString("saved.bmp"),"BMP");
}
