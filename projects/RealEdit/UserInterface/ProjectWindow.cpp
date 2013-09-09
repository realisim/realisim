/*
 *  Vierwer.cpp
 *  Realisim
 *
 *  Created by Pierre-Olivier Beaudoin on 10-05-30.
 */

#include "UserInterface/RealEdit3d.h"
#include "3d/Camera.h"
#include "UserInterface/MainWindow.h"
#include "UserInterface/ProjectWindow.h"

#include <QKeyEvent>
#include "QtGui/QFocusEvent"
#include "QtGui/QFrame"
#include "QtGui/QLayout"

using namespace realEdit;
using namespace realisim;
  using namespace treeD;
using namespace std;

ProjectWindow::ProjectWindow(QWidget* ipParent /*=0*/) : QWidget(ipParent),
  mController(*this),
  mViews()
{
  setFocusPolicy(Qt::StrongFocus);
  setWindowFlags(Qt::Window);

	QGridLayout* pGLyt = new QGridLayout( this );
  pGLyt->setContentsMargins(1, 1, 1, 1);
  pGLyt->setSpacing( 1 );
	
  Camera c;
	RealEdit3d* pV1 = new RealEdit3d( this, 0, mController);
  //c = pV1->getCamera();
  c.setOrthoProjection(20, 1, 10000);
  pV1->setCamera(c, false);
//  pV1->setCameraMode( Camera::ORTHOGONAL );
  pV1->setCameraOrientation( Camera::XY );
    
  RealEdit3d* pV2 = new RealEdit3d( this, pV1, mController);
  pV2->setCamera(c, false);
//  pV2->setCameraMode( Camera::ORTHOGONAL );
  pV2->setCameraOrientation( Camera::ZY );
  
	RealEdit3d* pV3 = new RealEdit3d (this, pV1, mController);
  pV3->setCamera(c, false);
//  pV3->setCameraMode( Camera::ORTHOGONAL );
  pV3->setCameraOrientation( Camera::XZ );

	RealEdit3d* pV4 = new RealEdit3d (this, pV1, mController);
  c.setPerspectiveProjection(60, 1.0, 0.005, 10000);
  pV4->setCamera(c, false);
  pV4->setCameraOrientation( Camera::FREE );

  mViews.push_back(pV4);
  mViews.push_back(pV1);
  mViews.push_back(pV2);
  mViews.push_back(pV3);
  
  pGLyt->addWidget(pV4, 0, 0, 1, 3);
  pGLyt->setRowStretch(0, 2);
  pGLyt->addWidget(pV1, 1, 0 );
  pGLyt->addWidget(pV2, 1, 1 );
  pGLyt->addWidget(pV3, 1, 2 );
}

ProjectWindow::~ProjectWindow()
{
}

//-----------------------------------------------------------------------------
void ProjectWindow::changeCurrentNode()
{
  for(unsigned int i = 0; i < mViews.size(); ++i)
    mViews[i]->changeCurrentNode();
}

//-----------------------------------------------------------------------------
void ProjectWindow::closeEvent(QCloseEvent* ipEvent)
{
  ipEvent->accept();
  emit aboutToClose(this);
}

//-----------------------------------------------------------------------------
void ProjectWindow::keyPressEvent(QKeyEvent* ipE)
{
	//On passe l'évenement au parent
	ipE->ignore();
  QWidget::keyPressEvent(ipE);
}

//-----------------------------------------------------------------------------
void ProjectWindow::updateUi()
{
  for(unsigned int i = 0; i < mViews.size(); ++i)
    mViews[i]->update();
}

