/*************************************************

 Purpose:
   This class will show all added images as icons
   and will also update the icon-images when
   we detect corners.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: MontageView.cpp,v 1.1 2004/12/21 23:00:48 cygnus78 Exp $

*************************************************/

#include "MontageView.h"

MontageView::MontageView( QWidget* parent, const char* name, WFlags f ) 
  : QIconView( parent, name, f ){}

