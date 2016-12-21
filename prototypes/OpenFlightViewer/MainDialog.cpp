/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "openFlight/DotExporter.h"
#include "3d/Camera.h"
#include "Definitions.h"
#include "FltImporter.h"
#include "MainDialog.h"
#include <QFileDialog>
#include <QLayout>
#include <QMenuBar>
#include <queue>
#include "Representations.h"
#include <string>

using namespace realisim;
using namespace math;
using namespace treeD;
using namespace std;


Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent)
{
    setFocusPolicy(Qt::StrongFocus);
}

Viewer::~Viewer()
{}

//------------------------------------------------------------------------------
void Viewer::draw()
{
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    showFps();
    
    for(size_t i = 0; i < mpScene->mToDraw.size(); ++i)
    {
        mpScene->mToDraw[i]->draw();
    }
}

//------------------------------------------------------------------------------
void Viewer::initializeGL()
{
    Widget3d::initializeGL();
    
    glCullFace(GL_BACK);
}

//------------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
    Widget3d::keyPressEvent(ipE);
    
    switch (ipE->key())
    {
        case Qt::Key_P: togglePolygonMode(); break;
        default: break;
    }
}

//------------------------------------------------------------------------------
void Viewer::togglePolygonMode()
{
    int polygonMode;
    glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
    
    GLenum e = polygonMode;
    switch (e)
    {
        case GL_LINE: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        case GL_FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
        default: break;
    }
}

//------------------------------------------------------------------------------
void Viewer::update()
{
    Widget3d::update();
}

//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
mpViewer(0),
mTimerId(0)
{
    resize(800, 600);
    
    QHBoxLayout* pLyt = new QHBoxLayout(this);
    pLyt->setMargin(5);
    mpViewer = new Viewer(this);
    pLyt->addWidget(mpViewer);
    setCentralWidget(mpViewer);

    //--- viewer
    mpViewer->setControlType( Widget3d::ctFree );
    Camera c = mpViewer->getCamera();
    Camera::Projection proj = c.getProjection();
    proj.mNear = 1.0; //1 m
    proj.mFar = 100000; // 100 km
    c.setProjection(proj);
    c.set( Point3d(0.0, -10.0, 0.0),
          Point3d(), Vector3d(0, 0, 1) );
    mpViewer->setAbsoluteUpVector(Widget3d::auvZ);
    mpViewer->setCamera( c, false );
    mpViewer->setScene(&mScene);
    

    createMenus();
    
    mTimerId = startTimer(15);
}

//-----------------------------------------------------------------------------
void MainDialog::createMenus()
{
    QMenuBar* pMenuBar = new QMenuBar(this);
    setMenuBar( pMenuBar );
    
    QMenu* pFile = pMenuBar->addMenu("File");
    pFile->addAction( QString("&New..."), this, SLOT( newFile() ),
                     QKeySequence::New );
    
    pFile->addAction( QString("&Open..."), this, SLOT( openFile() ),
                     QKeySequence::Open );
}

//-----------------------------------------------------------------------------
void MainDialog::openFile()
{
    OpenFlight::OpenFlightReader ofr;
    //ofr.enableDebug(true);
    //ofr.enableExternalReferenceLoading(false);
    
    QStringList filenamePaths = QFileDialog::getOpenFileNames(this, tr("Open Flt File"),
                                                        "../assets/",
                                                        tr("Flt (*.flt)"));
    
//    string filenamePath = "../assets/sample/nested_references/master/master.flt";
//    string filenamePath = "../assets/sample/nested_references2/db/1/12/123/1234/1234.flt";
//    string filenamePath = "../assets/sample/nested_references2/db/1/1.flt";
//    string filenamePath = "../assets/sample/nested_references2/db/1/12/12.flt";
//    string filenamePath = "/Users/po/Documents/travail/Simthetiq/assets/general_models/vehicles/airplanes/Military/Mig_21/Mig_21.flt";
    
    for(int i = 0; i < filenamePaths.count(); ++i)
    {
        OpenFlight::HeaderRecord *header = ofr.open( filenamePaths.at(i).toStdString() );
        
        if(!ofr.hasErrors())
        {
            if(ofr.hasWarnings())
            { cout << "Warning while opening flt file: " << endl << ofr.getAndClearLastWarnings(); }
            
            FltImporter fltImporter(header);
            mScene.addNode( fltImporter.getDefinitionRoot() );
            
            //cout << OpenFlight::toDotFormat( fltImporter.getOpenFlightRoot() );
        }
        else
        {
            delete header;
            cout << "Error while opening flt file: " << endl << ofr.getAndClearLastErrors();
        }
    }
}

//-----------------------------------------------------------------------------
void MainDialog::newFile()
{
    mScene.clear();
    update();
}

//-----------------------------------------------------------------------------
void MainDialog::timerEvent(QTimerEvent *ipE)
{
    if(ipE->timerId() == mTimerId)
    {
        mScene.update();
        
        mpViewer->update();
    }
}