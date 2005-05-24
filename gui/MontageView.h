/*************************************************

 Purpose:
   This class will show all added images as icons
   and will also update the icon-images when
   we detect corners.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: MontageView.h,v 1.3 2005/05/24 20:33:35 cygnus78 Exp $

*************************************************/

#ifndef MONTAGEVIEW_H
#define MONTAGEVIEW_H

#include <qiconview.h>
#include <qpopupmenu.h>
#include <qaction.h>

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
  void showContextMenu(QIconViewItem* item, const QPoint& pos);
  void removeAllItems();
  void removeItem();
  void openItem();

signals:
  void openImage( QString );
  void removedItem( QString );
  void removedAllItems();

private:
  void initActions();
  
  QPopupMenu m_item_menu;
  QAction* m_remove_action;
  QAction* m_remove_all_action;
  QAction* m_open_action;
  
};

#endif //MONTAGEVIEW_H
