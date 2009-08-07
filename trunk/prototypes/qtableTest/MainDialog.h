/*
 *  MainWindow.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include <QDialog>
#include <QItemDelegate>

class ComboBoxTableItem : public QItemDelegate
{
public: 
   ComboBoxTableItem(QObject* parent = 0);
   
   virtual QWidget* createEditor(QWidget* parent,
     const QStyleOptionViewItem& option, const QModelIndex & index ) const;
};

class MainDialog : public QDialog
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
				
protected:
};

#endif
