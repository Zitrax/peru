/*************************************************

 Purpose:
   This class will show all added images as icons
   and will also update the icon-images when
   we detect corners.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: MontageView.h,v 1.1 2004/12/21 23:00:48 cygnus78 Exp $

*************************************************/

#ifndef MONTAGEVIEW_H
#define MONTAGEVIEW_H

#include <qiconview.h>

class MontageView : public QIconView
{
public:
  MontageView( QWidget* parent, const char* name=0, WFlags f=0 );
  ~MontageView(){}

};

#endif //MONTAGEVIEW_H
