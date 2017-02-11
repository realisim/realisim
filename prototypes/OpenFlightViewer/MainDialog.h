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

#include "openFlight/OpenFlightReader.h"
#include <QDockWidget>
#include <QKeyEvent>
#include <QMainWindow>
#include <QTimerEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "3d/Widget3d.h"
#include "Scene.h"

class IGraphicNode;
namespace Representations{ class Representation; }

class Viewer : public realisim::treeD::Widget3d
{
public:
    Viewer(QWidget*);
    ~Viewer();
    
    friend class MainDialog;
    
    void update();
    
private:
    virtual void draw() override;
    virtual void initializeGL() override;
    virtual void keyPressEvent(QKeyEvent*) override;    
    void setScene(const Scene* ipScene) {mpScene = ipScene;}
    void togglePolygonMode();
    //void updateRepresentations();
    
    //--- data
    const Scene *mpScene;
    GLint mPolygonMode;
};


class MainDialog : public QMainWindow
{
    Q_OBJECT
public:
    MainDialog();
    ~MainDialog(){};
    
protected slots:
    void navigatorItemActivated(QTreeWidgetItem *, int);
    void newFile();
    void openFile();
    
protected:
    void createMenus();
    virtual void keyPressEvent(QKeyEvent*) override;
    void timerEvent(QTimerEvent*) override;
    void toggleFreeRunning();
    void refreshNavigator();
    void refreshNavigator(IGraphicNode*, QTreeWidgetItem*);
    void updateUi();
    
    Viewer* mpViewer;
    QDockWidget* mpToolsWidget;
    QTreeWidget* mpNavigator;
    std::map<QTreeWidgetItem*, IGraphicNode*> mNavigatorItemToGraphicNode; //this should be done with model/view from qt... but doing this quickly...
    
    //Data
    bool mFreeRunning;
    Scene mScene;
    int mTimerId;
};

#endif
