/*************************************************

 Purpose:
   Display and do simple manipulations on images

 Author:
   Daniel Bengtsson 2002, danielbe@ifi.uio.no

 Version:
   $Id: ImageWidget.cpp,v 1.4 2004/08/21 12:00:25 cygnus78 Exp $

*************************************************/

#include "ImageWidget.h"
#include <qcheckbox.h>
#include <qscrollview.h>
#include <iostream>
#include <qapplication.h>

ImageWidget::ImageWidget( QWidget *parent, const char *name) 
  : QWidget( parent, name), scaled(false)
{
  parent->installEventFilter(this);
}

ImageWidget::~ImageWidget(){}

void 
ImageWidget::displayImage(QImage& im)
{
  oImage=im;
  sImage=oImage;
  smoothScaleImage();

  repaint();
  // Check this later
  // bitBlt( this, 0, 0, &rendermap, 0, 0 ); 
}

void
ImageWidget::paintEvent( QPaintEvent *e)
{
  if(ccv::debug) std::cerr << "PaintEvent\n";
  if( sImage.isNull() ) 
    smoothScaleImage();

  if( !sImage.isNull() ) {
    if(ccv::debug) std::cerr << "Drawing image (" << sImage.width() << "," 
			     << sImage.height() << ")\n";
    QPainter qpainter( this );
    qpainter.drawImage( 0, 0, sImage );
  }
}

void 
ImageWidget::smoothScaleImage()
{
  QScrollView* parent = static_cast<QScrollView*>( this->parent() );

  QSize scaledsize;
  if( parent ) {

    if(scaled) {
      parent->setHScrollBarMode( QScrollView::AlwaysOff );
      parent->setVScrollBarMode( QScrollView::AlwaysOff );
      adjustSize();
    }

    scaledsize = parent->frameGeometry().size();
  }
  else 
    return;

  if(!oImage.isNull()){                                     // Do not scale null-images

    if(scaled && ( sImage.size() != scaledsize ) ){
      if(ccv::debug) std::cerr << "Scaling\n";
      sImage=oImage.smoothScale( scaledsize );
    }
    else { sImage=oImage; }
  }
  adjustSize();
}

QImage
ImageWidget::getImage()
{
  return oImage; // Maybe sImage ?
}

void
ImageWidget::setImage(QImage img)
{
  oImage = img;
  sImage = QImage();
  smoothScaleImage();
}

void
ImageWidget::setScaled(bool scale)
{
  scaled = scale;
  smoothScaleImage();
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
  return oImage.width()/static_cast<float>(sImage.width());
}

float
ImageWidget::getYScale()
{
  return oImage.height()/static_cast<float>(sImage.height());
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
  update();
}

bool
ImageWidget::saveImage( const QString& filename, const char* format )
{
  return oImage.save(filename,format);
}

QSize
ImageWidget::sizeHint() const
{
  return sImage.size();
}

bool
ImageWidget::eventFilter( QObject* target, QEvent* event )
{
  if( target == parentWidget() && event->type() == QEvent::Resize ) {
    if(ccv::debug) std::cerr << "Resize event\n";
    smoothScaleImage();
  }

  return QWidget::eventFilter( target, event );
}
