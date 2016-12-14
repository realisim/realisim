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
#include <qlayout.h>
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
    startTimer(15);
}

Viewer::~Viewer()
{
    mDefinitionIdToRepresentation.clear();
}

//------------------------------------------------------------------------------
void Viewer::draw()
{
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    for(size_t i = 0; i < mToDraw.size(); ++i)
    {
        mToDraw[i]->draw();
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
    mToDraw.clear();
    updateRepresentations();
    
    Widget3d::update();
}

//------------------------------------------------------------------------------
void Viewer::updateRepresentations()
{
    if(mpScene != nullptr)
    {
        deque<const Node*> q;
        q.push_back(mpScene->mpRoot);
        while(!q.empty())
        {
            const Node* n = q.front();
            q.pop_front();
            
            for(int i = 0; i < n->mChilds.size(); ++i)
            { q.push_back(n->mChilds[i]); }
            
            switch (n->mNodeType)
            {
                case Node::ntGroup: break;
                case Node::ntLibrary: break;
                case Node::ntModel:
                {
                    auto repIt = mDefinitionIdToRepresentation.find(n->mId);
                    if(repIt == mDefinitionIdToRepresentation.end())
                    {
                        Representations::Model* m = new Representations::Model((ModelNode*)n);
                        repIt = mDefinitionIdToRepresentation.insert( make_pair(n->mId, m) ).first;
                    }
                    mToDraw.push_back( repIt->second );
                } break;
                case Node::ntNode: break;
                case Node::ntOpenFlight: break;
                default: break;
            }
        }
    }
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

    mpViewer->setControlType( Widget3d::ctFree );
    Camera c = mpViewer->getCamera();
    c.set( Point3d(0.0, 0.0, 1),
          Point3d(), Vector3d(0, 1, 0) );
    mpViewer->setCamera( c, false );
    
    mpViewer->setScene(&mScene);
    
    mTimerId = startTimer(30);
    
    openFltFile();
}

//-----------------------------------------------------------------------------
void MainDialog::openFltFile()
{
    OpenFlight::OpenFlightReader ofr;
    ofr.enableDebug(true);
    
//    string filenamePath = "../assets/sample/nested_references/master/master.flt";
//    string filenamePath = "../assets/sample/nested_references2/db/1/12/123/1234/1234.flt";
//    string filenamePath = "../assets/sample/nested_references2/db/1/1.flt";
    string filenamePath = "../assets/sample/nested_references2/db/1/12/12.flt";
    FltImporter fltImporter( ofr.open( filenamePath ) );
    
    if(!ofr.hasErrors())
    {
        if(ofr.hasWarnings())
        { cout << "Warning while opening flt file: " << endl << ofr.getAndClearLastWarnings(); }
        
        cout << OpenFlight::toDotFormat( fltImporter.getOpenFlightRoot() );
        
        mScene.mpRoot->mChilds.push_back( fltImporter.getDefinitionRoot() );
    }
    else
    {
        cout << "Error while opening flt file: " << endl << ofr.getAndClearLastErrors();
    }
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

//-----------------------------------------------------------------------------
void MainDialog::updateScene()
{
    deque<const Node*> q;
    q.push_back(mScene.mpRoot);
    while(!q.empty())
    {
        const Node* n = q.front();
        q.pop_front();
        
        for(int i = 0; i < n->mChilds.size(); ++i)
        { q.push_back(n->mChilds[i]); }
        
        switch (n->mNodeType)
        {
            case Node::ntGroup: //update transformation if dirty
                break;
            case Node::ntLibrary:
                // load unloaded data
                break;
            case Node::ntModel:
                //update transformation if dirty
                break;
            case Node::ntNode: break;
            case Node::ntOpenFlight:
                //update transformation if dirty
                break;
            default: break;
        }
        
    }
}