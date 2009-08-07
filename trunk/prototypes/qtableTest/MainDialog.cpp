/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "MainDialog.h"
#include <QComboBox>
#include <qheaderview.h>
#include <qlayout.h>
#include <qtablewidget.h>

ComboBoxTableItem::ComboBoxTableItem(QObject* p/*=0*/) : QItemDelegate(p)
{}

QWidget* ComboBoxTableItem::createEditor(QWidget* parent,
     const QStyleOptionViewItem& option, const QModelIndex & index ) const
{
  QComboBox* c = new QComboBox(parent);
  c->addItem( "mâle" );
  c->addItem( "femme" );
  c->addItem( "ortho" );
  
  return c;
}

MainDialog::MainDialog() 
: QDialog()
{
  QHBoxLayout* pMainLayout = new QHBoxLayout(this);
  QTableWidget* t = new QTableWidget(10, 6, this);
  t->setAlternatingRowColors(true);
  t->verticalHeader()->hide();
  t->setShowGrid(false);

  for(unsigned int i = 0; i < 6;	 ++i)
    for(unsigned int j = 0; j < 10; ++j)
    {
      t->setItem(i, j, new QTableWidgetItem());
    }
  t->setItemDelegateForColumn(4, new ComboBoxTableItem(t));
  t->item(1, 0)->setText("asdasdasdafdhgfdghkjfglsdkfgj");
  
  pMainLayout->addWidget(t);

	//showFullScreen();
}