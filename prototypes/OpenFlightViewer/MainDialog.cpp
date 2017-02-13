/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "3d/Camera.h"
#include "Definitions.h"
#include "MainDialog.h"
#include "MessageQueue.h"
#include <QFileDialog>
#include <QHeaderView>
#include <QLayout>
#include <QMenuBar>
#include <queue>
#include "Representations.h"
#include <string>
#include "utils/Timer.h"
#include "3d/openGlHeaders.h"

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
    
    // devrait tout remplacer ce qui suit par
    // mpScene->draw...
    
    for(size_t i = 0; i < mpScene->mDefaultLayer.size(); ++i)
    {
        mpScene->mDefaultLayer[i]->draw();
    }

    
    auto itLayer = mpScene->mLayers.begin();
    
    if (itLayer != mpScene->mLayers.end())
    {
        {
            glEnable(GL_STENCIL_TEST);
            glClear(GL_STENCIL_BUFFER_BIT);

            glEnable(GL_DEPTH_TEST);
            glStencilFunc(GL_ALWAYS,1,1);
            glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

            //draw layer 1 - base layer
            //assert(itLayer->first == 1);
            const vector<Representations::Representation*> &layer = itLayer->second;
            for (size_t i = 0; i < layer.size(); ++i)
            {
                layer[i]->draw();
            }
        }


        // draw other layers
        {
            glEnable(GL_STENCIL_TEST);
            glDisable(GL_DEPTH_TEST);
            glStencilFunc(GL_EQUAL,1,1);
            glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

            ++itLayer;        
            for(; itLayer != mpScene->mLayers.end(); ++itLayer)
            {
                const vector<Representations::Representation*> &layer = itLayer->second;
                for (size_t i = 0; i < layer.size(); ++i)
                {
                    layer[i]->draw();
                }
            }
        }

        glDisable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
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

            connect(mpNavigator, SIGNAL(itemActivated(QTreeWidgetItem *, int)), 
                this, SLOT(navigatorItemActivated(QTreeWidgetItem *, int)));

            pHbox->addWidget(mpNavigator);
        }

        mpToolsWidget->setWidget(pToolsWidget);
    }

    createMenus();
    
    //register done queue to fileStreaming
    using placeholders::_1;
    mFileLoadingDoneQueue.setProcessingFunction(
        std::bind( &MainDialog::processFileLoadingDoneMessage, this, _1));
    mFileStreamer.registerDoneQueue(this, &mFileLoadingDoneQueue);
    
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
    QStringList filenamePaths = QFileDialog::getOpenFileNames(
        this, tr("Open Flt File"),
        "../assets/",
        tr("Flt (*.flt)"));

    for(int i = 0; i < filenamePaths.count(); ++i)
    {
        FileStreamer::Request *r = new FileStreamer::Request(this);
        r->mRequestType = FileStreamer::rtLoadFlt;
        r->mFilenamePath = filenamePaths.at(i).toStdString();
        mFileStreamer.postRequest(r);
    }
}

//-----------------------------------------------------------------------------
void MainDialog::processFileLoadingDoneMessage(MessageQueue::Message* ipMessage)
{
    FileStreamer::DoneRequest *d = (FileStreamer::DoneRequest *)ipMessage;
    printf("MainDialog - file %s was loaded\n", d->mFilenamePath.c_str());
    
    switch(d->mRequestType)
    {
        case FileStreamer::rtLoadFlt: mScene.addNode((IGraphicNode *)d->mpData ); break;
        default: break;
    }
    
    refreshNavigator();
}

//-----------------------------------------------------------------------------
void MainDialog::navigatorItemActivated(QTreeWidgetItem *ipItem, int iColumn)
{
    //find corresponding graphic node
    auto itGraphicNode = mNavigatorItemToGraphicNode.find(ipItem);
    if (itGraphicNode != mNavigatorItemToGraphicNode.end())
    {
        IRenderable* r = dynamic_cast<IRenderable*>(itGraphicNode->second);
        if (r)
        {
            r->setAsVisible( !r->isVisible() );
        }
    }
    updateUi();
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
    mNavigatorItemToGraphicNode.clear();
    
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

            //insert in map to track relationship from tree item to IGraphicNode
            mNavigatorItemToGraphicNode.insert( make_pair(item, ipNode) );
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

    wglSwapIntervalEXT(mFreeRunning ? 0 : 1);

    killTimer(mTimerId);
    mTimerId = mFreeRunning ? startTimer(0) : startTimer(15);
}

//-----------------------------------------------------------------------------
void MainDialog::timerEvent(QTimerEvent *ipE)
{
    if(ipE->timerId() == mTimerId)
    {
        mFileLoadingDoneQueue.processNextMessage();
        
        mScene.update();
        
        mpViewer->update();
    }
}

//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
}