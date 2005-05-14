/*************************************************

 Purpose:
   This class will show all added images as icons
   and will also update the icon-images when
   we detect corners.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: MontageView.h,v 1.2 2005/05/14 00:11:03 cygnus78 Exp $

*************************************************/

#ifndef MONTAGEVIEW_H
#define MONTAGEVIEW_H

#include <qiconview.h>

class MontageView : public QIconView
{
Q_OBJECT

public:
  MontageView( QWidget* parent, const char* name=0, WFlags f=0 );
  ~MontageView(){}

protected:

  // Disable dragging
  virtual void startDrag() { return; }

private slots:
  void itemDoubleClicked( QIconViewItem* item );

signals:
  void openImage( QString );

};

#endif //MONTAGEVIEW_H
