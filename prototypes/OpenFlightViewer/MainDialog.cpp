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
    
    glClearColor(0.04f, 0.04f, 0.04f, 0.0f);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_COLOR_MATERIAL);

    //useful for lights
    const float aCoeff = 0.2f;
    const float dCoeff = 0.7f;
    GLfloat shininess[] = {80.0};
    GLfloat ambiant[]   = {aCoeff, aCoeff, aCoeff, 1.0f};
    GLfloat diffuse[]   = {dCoeff, dCoeff, dCoeff, 1.0};
    GLfloat specular[]  = {0.0, 0.0, 0.0, 1.0};

    const float matACoeff = 0.25f;
    const float matDCoeff = 0.7f;
    GLfloat mat_ambiant[] = {matACoeff, matACoeff, matACoeff, 1.0f};
    GLfloat mat_diffuse[] = {matDCoeff, matDCoeff, matDCoeff, 1.0f};
    GLfloat mat_specular[]  = {0.0f, 0.0f, 0.0f, 1.0f};

    //define material props
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambiant);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    //init lights
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiant);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
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
mHub(),
mpScene(nullptr),
mpViewer(nullptr),
mTimerId(0)
{
    resize(800, 600);
    
    //init data
    
    // initialise hub
    mHub.setFileStreamer(&mFileStreamer);
    mHub.setGpuStreamer(&mGpuStreamer);
    mpScene = new Scene(&mHub);
    
    //register done queue to fileStreaming
    using placeholders::_1;
    mFileLoadingDoneQueue.setProcessingFunction(
        std::bind( &MainDialog::processFileLoadingDoneMessage, this, _1));
    mFileStreamer.registerDoneQueue(this, &mFileLoadingDoneQueue);
    
    //--- init ui
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
    c.set( Point3d(0.0, -1000.0, 1000.0),
          Point3d(), Vector3d(0, 0, 1) );
    mpViewer->setAbsoluteUpVector(Widget3d::auvZ);
    mpViewer->setCamera( c, false );
    mpViewer->setScene(mpScene);
    
    //--- tools widget
    mpToolsWidget = new QDockWidget(this);
    mpToolsWidget->setFloating(true);
    mpToolsWidget->setWindowTitle("Tools");
    {
        QWidget *pToolsWidget = new QWidget(mpToolsWidget);
        pToolsWidget->resize(400, 900);
        QHBoxLayout *pHbox = new QHBoxLayout(pToolsWidget);
        {
            mpNavigator = new QTreeWidget(mpToolsWidget);
            mpNavigator->header()->hide();
            mpNavigator->setColumnCount(1);

            connect(mpNavigator, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), 
                this, SLOT(navigatorCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)) );
            connect(mpNavigator, SIGNAL(itemActivated(QTreeWidgetItem *, int)), 
                this, SLOT(navigatorItemActivated(QTreeWidgetItem *, int)));

            pHbox->addWidget(mpNavigator);
        }

        mpToolsWidget->setWidget(pToolsWidget);
    }    

    createMenus();
    
//{
//        MessageQueue mq;
//        mq.startInThread();
//        std::this_thread::sleep_for( std::chrono::seconds(4) );
//        mq.post( new MessageQueue::Message(this) );
//        mq.post( new MessageQueue::Message(this) );
//        mq.post( new MessageQueue::Message(this) );
//        mq.post( new MessageQueue::Message(this) );
//        std::this_thread::sleep_for( std::chrono::seconds(2) );
//}

    mTimerId = startTimer(15);
}

//-----------------------------------------------------------------------------
void MainDialog::centerCameraOn(realisim::math::Point3d iC)
{
    Camera c = mpViewer->getCamera();
    Point3d e = iC + Vector3d(0.0, -100, 100);
    c.set( e, iC, Vector3d(0.0, 0.0, 1.0) );

    mpViewer->setCamera(c);
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
    case Qt::Key_C: resetCamera(); break;
    default: break;
    }
}

//-----------------------------------------------------------------------------
void MainDialog::openFile()
{
    QStringList filenamePaths = QFileDialog::getOpenFileNames(
        this, tr("Open Flt File"),
        //"../assets/",
        "F:/aXion_world/ModelsSrc/airports/omdb",
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
    FileStreamer::Request *d = (FileStreamer::Request *)ipMessage;
    //printf("MainDialog - file %s was loaded\r", d->mFilenamePath.c_str());
    
    switch(d->mRequestType)
    {
        case FileStreamer::rtLoadFlt: mpScene->addNode((IGraphicNode *)d->mpData ); break;
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
            centerCameraOn( r->getPositionnedAABB().getCenter() );
        }
    }
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::navigatorCurrentItemChanged(QTreeWidgetItem *ipCurrent, QTreeWidgetItem *ipPrevious)
{
    //find corresponding graphic node
    auto itCurrentGraphicNode = mNavigatorItemToGraphicNode.find(ipCurrent);
    auto itPreviousGraphicNode = mNavigatorItemToGraphicNode.find(ipPrevious);

    if (itCurrentGraphicNode != mNavigatorItemToGraphicNode.end())
    {
        IRenderable* r = dynamic_cast<IRenderable*>(itCurrentGraphicNode->second);
        if (r)
        { r->setBoundingBoxVisible( true ); }
    }

    if (itPreviousGraphicNode != mNavigatorItemToGraphicNode.end())
    {
        IRenderable* r = dynamic_cast<IRenderable*>(itPreviousGraphicNode->second);
        if (r)
        { r->setBoundingBoxVisible( false ); }
    }
    updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::newFile()
{
    mpScene->clear();
    refreshNavigator();
    update();
}

//------------------------------------------------------------------------------
void MainDialog::refreshNavigator()
{
    mpNavigator->clear();
    mNavigatorItemToGraphicNode.clear();
    
    IGraphicNode *pRoot = mpScene->getRoot();

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

            QString itemText = QString::fromStdString(ipNode->mName);

            // show instantiated nodes...
            IDefinition *def = dynamic_cast<IDefinition*>(ipNode);
            if(def && def->isInstantiated())
            { itemText += " (instance)"; }

            item->setText( 0, itemText );
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
void MainDialog::resetCamera()
{
    Camera c = mpViewer->getCamera();
    c.set( Point3d(0.0, -1000.0, 1000.0),
        Point3d(), Vector3d(0, 0, 1) );
    mpViewer->setCamera( c, false );
}

//------------------------------------------------------------------------------
void MainDialog::toggleFreeRunning()
{
    mFreeRunning = !mFreeRunning;

#ifdef WIN32
    wglSwapIntervalEXT(mFreeRunning ? 0 : 1);
#endif
    killTimer(mTimerId);
    mTimerId = mFreeRunning ? startTimer(0) : startTimer(15);
}

//-----------------------------------------------------------------------------
void MainDialog::timerEvent(QTimerEvent *ipE)
{
    if(ipE->timerId() == mTimerId)
    {
        mFileLoadingDoneQueue.processNextMessage();
        
        mpScene->update();
        
        mpViewer->update();
    }
}

//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
}