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

#include "Broker.h"
#include "FileStreamer.h"
#include "GpuStreamer.h"
#include "Hub.h"
#include "MessageQueue.h"
#include <QDockWidget>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QOpenGLContext>
#include <QTimerEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWindow>
#include "3d/Widget3d.h"
#include "Scene.h"

class IGraphicNode;
namespace Representations{ class Representation; }

class SharedGlWindow : public QWindow
{
public:
    explicit SharedGlWindow(QWindow* ipParent=0);

    HGLRC openGlNativeHandle() const;
    HDC openGlNativeDC() const;

protected:
    //void exposeEvent(QExposeEvent *event) override;

    QOpenGLContext *mpContext;
};

//-------------------------------------------------------------
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
    void navigatorCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void newFile();
    void openFile();
    
protected:
    void centerCameraOn(realisim::math::Point3d, double iRadius);
    void createInfoPanel(QWidget*);
    void createMenus();
    virtual void keyPressEvent(QKeyEvent*) override;
    void fillInfoLabel();
    void processFileLoadingDoneMessage(MessageQueue::Message*);
    void timerEvent(QTimerEvent*) override;
    void toggleFreeRunning();
    void refreshNavigator();
    void refreshNavigator(IGraphicNode*, QTreeWidgetItem*);
    void resetCamera();
    void updateUi();
    void updateUiAtHighFrequency();
    
    // ui
    Viewer* mpViewer;
    SharedGlWindow* mpSharedGlWindow;
    QDockWidget* mpToolsWidget;
    QTreeWidget* mpNavigator;
    QWidget* mpInfoPanel;
    QLabel* mpInfoLabel;
    QLabel* mpStatsPerFrameLabel;
    std::map<QTreeWidgetItem*, IGraphicNode*> mNavigatorItemToGraphicNode; //this should be done with model/view from qt... but doing this quickly...
    
    //Data
    bool mFreeRunning;
    Hub mHub;
    Scene *mpScene;
    int mTimerId;
  
    Broker mBroker;
    //threading
    FileStreamer mFileStreamer;
    GpuStreamer mGpuStreamer;
    MessageQueue mFileLoadingDoneQueue;
};

#endif
