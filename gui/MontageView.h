/*************************************************

 Purpose:
   This class will show all added images as icons
   and will also update the icon-images when
   we detect corners.

 Author:
   Daniel Bengtsson, daniel@bengtssons.info

 Version:
   $Id: MontageView.h,v 1.5 2005/06/20 22:09:34 cygnus78 Exp $

*************************************************/

#ifndef MONTAGEVIEW_H
#define MONTAGEVIEW_H

#include <qiconview.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qpointarray.h>

class MontageView : public QIconView
{
Q_OBJECT

public:
  MontageView( QWidget* parent, const char* name=0, WFlags f=0 );
  ~MontageView(){}

  /**
   * Draws points on all item-thumbnails at the positions in the pointarray.
   * @param pa This contains coordinates where marks should be made
   * @param corners This list should contain the number of points that should be drawn on each image.
   */
  void drawPoints(const QPointArray& pa, QValueList<int>& corners );

  /** Resets the icons to the original images */
  void resetIcons();

private slots:
  void itemDoubleClicked( QIconViewItem* item );
  void showContextMenu(QIconViewItem* item, const QPoint& pos);
  void removeItem();
  void removeAllItems();
  void openItem();

signals:
  void openImage( QString );
  void removedItem( QString );
  void removedAllItems();

private:
  void initActions();
  
  bool m_dirty;
  QPopupMenu m_item_menu;
  QAction* m_remove_action;
  QAction* m_remove_all_action;
  QAction* m_open_action;
  
};

#endif //MONTAGEVIEW_H
