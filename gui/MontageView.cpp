/*************************************************

 Purpose:
   This class will show all added images as icons
   and will also update the icon-images when
   we detect corners.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: MontageView.cpp,v 1.5 2005/05/25 22:03:54 cygnus78 Exp $

*************************************************/

#include "MontageView.h"

MontageView::MontageView( QWidget* parent, const char* name, WFlags f ) 
  : QIconView( parent, name, f ),
    m_remove_action(0),
    m_remove_all_action(0),
    m_open_action(0)

{
  initActions();

  setItemsMovable(false);

  connect( this, SIGNAL( doubleClicked( QIconViewItem* ) ), 
	   this,   SLOT( itemDoubleClicked( QIconViewItem* ) ));

  connect( this, SIGNAL( contextMenuRequested(QIconViewItem*, const QPoint&) ),
	   this,   SLOT( showContextMenu(QIconViewItem*, const QPoint&) ) );
}

void MontageView::itemDoubleClicked( QIconViewItem* item )
{
  emit openImage( item->text() );
}

void MontageView::openItem()
{
  if( currentItem() )
    emit openImage( currentItem()->text() );
}

void MontageView::showContextMenu(QIconViewItem* item, const QPoint& pos)
{
  bool have_item = item ? true : false;

  m_remove_action->setEnabled(have_item);
  m_open_action  ->setEnabled(have_item);
  m_remove_all_action->setEnabled(count() ? true : false );

  m_item_menu.exec(pos);
}

void MontageView::initActions()
{
  m_remove_action     = new QAction( tr("Remove item from calibration list"), 
				     tr("Remove"), CTRL+Key_R, this );
  m_remove_all_action = new QAction( tr("Remove all images from calibration list"), 
				     tr("Remove All "), CTRL+SHIFT+Key_R, this );
  m_open_action       = new QAction( tr("Open this image"), 
				     tr("Open"), CTRL+Key_O, this );
  
  connect( m_remove_action,     SIGNAL( activated() ), this, SLOT( removeItem()     ));
  connect( m_remove_all_action, SIGNAL( activated() ), this, SLOT( removeAllItems() ));
  connect( m_open_action,       SIGNAL( activated() ), this, SLOT( openItem()       ));

  m_remove_action    ->addTo( &m_item_menu );
  m_remove_all_action->addTo( &m_item_menu );
  m_open_action      ->addTo( &m_item_menu );

  m_remove_all_action->setEnabled(true);
  m_remove_action    ->setEnabled(false);
  m_open_action      ->setEnabled(false);
}

void MontageView::removeItem()
{
  QIconViewItem* item = currentItem(); 
  if( item ) {
    emit removedItem( item->text() );
    delete item;
    arrangeItemsInGrid();
  }
}

void MontageView::removeAllItems()
{
  clear();
  emit removedAllItems();
}
