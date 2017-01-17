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
#include <QHeaderView>
#include <QLayout>
#include <QMenuBar>
#include <queue>
#include "Representations.h"
#include <string>
#include "utils/Timer.h"

using namespace realisim;
using namespace math;
using namespace treeD;
using namespace std;


Viewer::Viewer(QWidget* ipParent /*=0*/) : Widget3d(ipParent),
mPolygonMode(GL_FILL)
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
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

//------------------------------------------------------------------------------
void Viewer::keyPressEvent(QKeyEvent* ipE)
{
    Widget3d::keyPressEvent(ipE);
    
    switch (ipE->key())
    {
        case Qt::Key_P: togglePolygonMode(); break;
        default: ipE->ignore(); break;
    }
}

//------------------------------------------------------------------------------
void Viewer::togglePolygonMode()
{
    // Not able to query polygon_mode?!? not in the doc anymore...
    //GLint64 polygonMode = 0;
    //glGetInteger64v(GL_POLYGON_MODE, &polygonMode);
    
    switch (mPolygonMode)
    {
        case GL_LINE: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); mPolygonMode = GL_FILL; break;
        case GL_FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); mPolygonMode = GL_LINE; break;
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
    
    //--- tools widget
    mpToolsWidget = new QDockWidget(this);
    mpToolsWidget->setFloating(true);
    mpToolsWidget->setWindowTitle("Tools");
    {
        QWidget *pToolsWidget = new QWidget(mpToolsWidget);
        QHBoxLayout *pHbox = new QHBoxLayout(pToolsWidget);
        {
            mpNavigator = new QTreeWidget(mpToolsWidget);
            mpNavigator->header()->hide();
            mpNavigator->setColumnCount(1);

            pHbox->addWidget(mpNavigator);
        }

        mpToolsWidget->setWidget(pToolsWidget);
    }

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

//------------------------------------------------------------------------------
void MainDialog::keyPressEvent(QKeyEvent* ipE)
{
    switch (ipE->key())
    {
    case Qt::Key_F: toggleFreeRunning(); break;
    default: break;
    }
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
        realisim::utils::Timer __t;
        OpenFlight::HeaderRecord *header = ofr.open( filenamePaths.at(i).toStdString() );
        printf("Temps pour lire %s: %.4f (sec)\n", filenamePaths.at(i).toStdString().c_str(), __t.getElapsed() );
        
        if(!ofr.hasErrors())
        {
            if(ofr.hasWarnings())
            { cout << "Warning while opening flt file: " << endl << ofr.getAndClearLastWarnings(); }
            
            FltImporter fltImporter(header);
            mScene.addNode( fltImporter.getGraphicNodeRoot() );
            
            //cout << OpenFlight::toDotFormat( fltImporter.getOpenFlightRoot() );
        }
        else
        {
            delete header;
            cout << "Error while opening flt file: " << endl << ofr.getAndClearLastErrors();
        }
    }

    refreshNavigator();
}

//-----------------------------------------------------------------------------
void MainDialog::newFile()
{
    mScene.clear();
    refreshNavigator();
    update();
}

//------------------------------------------------------------------------------
void MainDialog::refreshNavigator()
{
    mpNavigator->clear();
    
    IGraphicNode *pRoot = mScene.getRoot();

    refreshNavigator(pRoot, nullptr);
}

//------------------------------------------------------------------------------
void MainDialog::refreshNavigator(IGraphicNode *ipNode, QTreeWidgetItem *ipParentItem)
{
    if (ipNode != nullptr)
    {
        QTreeWidgetItem *currentParent = ipParentItem;
        if (currentParent == nullptr)
        {
            currentParent = new QTreeWidgetItem(mpNavigator);
            currentParent->setText( 0, "Root" );
            mpNavigator->insertTopLevelItem(0, currentParent);
        }
        else
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(ipParentItem);
            item->setText( 0, QString::fromStdString(ipNode->mName) );
            currentParent = item;
        }


        for (size_t i = 0; i < ipNode->mChilds.size(); ++i)
        {
            refreshNavigator( ipNode->mChilds[i], currentParent );
        }
    }
}

//------------------------------------------------------------------------------
void MainDialog::toggleFreeRunning()
{
    mFreeRunning = !mFreeRunning;
    killTimer(mTimerId);
    mTimerId = mFreeRunning ? startTimer(0) : startTimer(15);
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