/*************************************************

 Purpose:
   This class will show all added images as icons
   and will also update the icon-images when
   we detect corners.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: MontageView.cpp,v 1.2 2005/05/14 00:11:03 cygnus78 Exp $

*************************************************/

#include "MontageView.h"

MontageView::MontageView( QWidget* parent, const char* name, WFlags f ) 
  : QIconView( parent, name, f )
{
  if( parent )
    setMinimumSize( parent->size() );

  connect( this, SIGNAL( doubleClicked( QIconViewItem* ) ), 
	   this,   SLOT( itemDoubleClicked( QIconViewItem* ) ));
}

void MontageView::itemDoubleClicked( QIconViewItem* item )
{
  emit openImage( item->text() );
}
