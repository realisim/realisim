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
#include <QKeyEvent>
#include <QMainWindow>
#include <QTimerEvent>
#include "3d/Widget3d.h"
#include "Scene.h"

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
    void updateRepresentations();
    
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
    void newFile();
    void openFile();
    
protected:
    void createMenus();
    virtual void keyPressEvent(QKeyEvent*) override;
    void timerEvent(QTimerEvent*) override;
    void toggleFreeRunning();
    
    Viewer* mpViewer;
    
    //Data
    bool mFreeRunning;
    Scene mScene;
    int mTimerId;
};

#endif
