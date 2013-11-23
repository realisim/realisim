/*
 *  MainWindow.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "3d/Camera.h"
#include "3d/Sprite.h"
#include "3d/Texture.h"
#include "3d/Utilities.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainDialog.h"
#include "qbuttongroup.h"
#include "qcheckbox.h"
#include "qcombobox.h"
#include "qgridlayout.h"
#include "QKeyEvent"
#include "qlabel.h"
#include "qlayout.h"
#include "qlineedit.h"
#include "qlistwidget.h"
#include "QFile.h"
#include "qpushbutton.h"
#include "qradiobutton.h"
#include "qvalidator.h"
#include "Viewer.h"

using namespace realisim;
  using namespace math;
  using namespace openCL;
  using namespace treeD;
  
static const int kIterationInterval = 25; //ms
  
//-----------------------------------------------------------------------------
MainDialog::MainDialog() : QMainWindow(),
  mpViewer(0),
  mParticuleSystem(),
  mTimerId(0),
  mpLeftPanel(0),
  mpMaxParticules(0),
  mpMaxInitialVelocity(0),
  mpMaxInitialLife(0),
  mpDecayEnabled(0),
  mp2d(0),
  mp3d(0),
  mpComputingMode(0),
  mpGravityHolesList(0),
  mpGravityHoleForce(0)
{
  resize(800, 600);
  
  QFrame* pMainFrame = new QFrame(this);
  setCentralWidget(pMainFrame);
  
  QHBoxLayout* pLyt = new QHBoxLayout(pMainFrame);
  pLyt->setMargin(2);
  pLyt->setSpacing(5);
  
  //--- on bricole le panneau de gauche
  mpLeftPanel = new QFrame(pMainFrame);
  QGridLayout* pLeftPanelLyt = new QGridLayout(mpLeftPanel);
  pLeftPanelLyt->setMargin(0);
  pLeftPanelLyt->setSpacing(5);
  
  int row = 0;
  //max particules
  QLabel* pMaxParticules = new QLabel("Max. particules: ", mpLeftPanel);
  mpMaxParticules = new QLineEdit(mpLeftPanel);
  QIntValidator* v = new QIntValidator(0, 16000000, mpMaxParticules);
  mpMaxParticules->setValidator(v);
  connect(mpMaxParticules, SIGNAL(textEdited(const QString&)),
    this, SLOT(maxParticulesChanged(const QString&)));
  pLeftPanelLyt->addWidget(pMaxParticules, row, 0);
  pLeftPanelLyt->addWidget(mpMaxParticules, row, 1);
  
  //Max initial velocity
  QLabel* pMaxInitialVel = new QLabel("Max. initial velocity: ", mpLeftPanel);
  mpMaxInitialVelocity = new QLineEdit(mpLeftPanel);
  QIntValidator* v1 = new QIntValidator(0, 16000000, mpMaxInitialVelocity);
  mpMaxInitialVelocity->setValidator(v1);
  connect(mpMaxInitialVelocity, SIGNAL(textEdited(const QString&)),
    this, SLOT(maxInitialVelocityChanged(const QString&)));
  pLeftPanelLyt->addWidget(pMaxInitialVel, ++row, 0);
  pLeftPanelLyt->addWidget(mpMaxInitialVelocity, row, 1);
  
  //Max Inital Life
  QLabel* pMaxInitialLife = new QLabel("Max. initial life: ", mpLeftPanel);
  mpMaxInitialLife = new QLineEdit(mpLeftPanel);
  QIntValidator* v2 = new QIntValidator(0, 16000000, mpMaxInitialLife);
  mpMaxInitialLife->setValidator(v2);
  connect(mpMaxInitialLife, SIGNAL(textEdited(const QString&)),
    this, SLOT(maxInitialLifeChanged(const QString&)));
  pLeftPanelLyt->addWidget(pMaxInitialLife, ++row, 0);
  pLeftPanelLyt->addWidget(mpMaxInitialLife, row, 1);
  
  //decay enable
  QLabel* pDecayEnabled = new QLabel("Decay enabled: ", mpLeftPanel);
  mpDecayEnabled = new QCheckBox(mpLeftPanel);
  connect(mpDecayEnabled, SIGNAL(clicked(bool)),
    this, SLOT(decayEnabledClicked(bool)));
  pLeftPanelLyt->addWidget(pDecayEnabled, ++row, 0);
  pLeftPanelLyt->addWidget(mpDecayEnabled, row, 1);
  
  //gravity holes
  mpGravityHolesList = new QListWidget(mpLeftPanel);
  pLeftPanelLyt->addWidget(mpGravityHolesList, ++row, 0, 1, 2);
  connect(mpGravityHolesList, SIGNAL(currentRowChanged(int)),
    this, SLOT(gravityHoleSelectionChanged(int)));
  
	QHBoxLayout* pAddRemoveGHLyt = new QHBoxLayout(mpLeftPanel);
  {
    QPushButton* pAddGH = new QPushButton("+", mpLeftPanel);
    pAddRemoveGHLyt->addWidget(pAddGH);
    connect(pAddGH, SIGNAL(clicked()), this, SLOT(addGravityHole()));
    QPushButton* pRemoveGH = new QPushButton("-", mpLeftPanel);
    pAddRemoveGHLyt->addWidget(pRemoveGH);
    connect(pRemoveGH, SIGNAL(clicked()), this, SLOT(removeGravityHole()));
  }
  pLeftPanelLyt->addLayout(pAddRemoveGHLyt, ++row, 0);
  
  //gravity hole force
  QLabel* pGravForce = new QLabel("Force: ", mpLeftPanel);
  mpGravityHoleForce = new QLineEdit(mpLeftPanel);
  QIntValidator* v3 = new QIntValidator(-1000000, 1000000, mpGravityHoleForce);
  mpGravityHoleForce->setValidator(v3);
  connect(mpGravityHoleForce, SIGNAL(textEdited(const QString&)),
    this, SLOT(gravityHoleForceChanged(const QString&)));
  pLeftPanelLyt->addWidget(pGravForce, ++row, 0);
  pLeftPanelLyt->addWidget(mpGravityHoleForce, row, 1);
 
  pLeftPanelLyt->setRowStretch(++row, 1);

  //computing mode
  mpComputingMode = new QComboBox(mpLeftPanel);
  mpComputingMode->insertItem(ParticuleSystem::cmCPU, "CPU");
  mpComputingMode->insertItem(ParticuleSystem::cmOpenCLCPU, "OpenCL CPU");
  mpComputingMode->insertItem(ParticuleSystem::cmOpenCLGPU, "OpenCL GPU");
  connect(mpComputingMode, SIGNAL(activated(int)),
    this, SLOT(computingModeChanged(int)));
  pLeftPanelLyt->addWidget(mpComputingMode, ++row, 0, 1, 2);
  
  //camera mode 2d vs 3d
  QLabel* pCameraMode = new QLabel("Camera Mode: ", mpLeftPanel);
  mp2d = new QRadioButton("2d", mpLeftPanel);
  mp3d = new QRadioButton("3d", mpLeftPanel);
  QButtonGroup* pCameraModeGrp = new QButtonGroup(mpLeftPanel);
  pCameraModeGrp->addButton(mp2d, 0);
  pCameraModeGrp->addButton(mp3d, 1);
  mp2d->setChecked(true);
  QHBoxLayout* pCameraModeHLyt = new QHBoxLayout(mpLeftPanel);
  pCameraModeHLyt->addWidget(mp2d);
  pCameraModeHLyt->addWidget(mp3d);
  connect(pCameraModeGrp, SIGNAL(buttonClicked(int)), this,
    SLOT(cameraModeChanged(int)));
  pLeftPanelLyt->addWidget(pCameraMode, ++row, 0);
  pLeftPanelLyt->addLayout(pCameraModeHLyt, row, 1);
  
  //ajoute le left panel au layout principale
  pLyt->addWidget(mpLeftPanel, 1);
  
  //--- on ajoute le viewer
  mpViewer = new Viewer(pMainFrame);
  connect(mpViewer, SIGNAL(gravityHoleSelected(unsigned int)), 
    this, SLOT(gravityHoleSelected(unsigned int)));
  connect(mpViewer, SIGNAL(hidePanel(bool)), 
    this, SLOT(hidePanel(bool)));
    
  mpViewer->setCameraOrientation(Camera::XY);
  mpViewer->setParticuleSystem(&mParticuleSystem);
  pLyt->addWidget(mpViewer, 6);
  
  //set la camera
  Camera c = mpViewer->getCamera();
  c.setOrthoProjection(20, 1, 10000);
  c.setZoom(30);
	mpViewer->setCamera(c, false);
  
  //TODO a enlever lorsque le UI sera fonctionnel.
  mParticuleSystem.setNumberOfParticules(400000);
  mParticuleSystem.setMaximumInitialVelocity(5);
  mParticuleSystem.setMaximumInitialLife(2000);
  mParticuleSystem.setColor(QColor(88, 13, 255));
  mParticuleSystem.initialize();
  mParticuleSystem.addGravityHole(Point3d(-110.0, 75.0, 0.0), 2000);
  //mParticuleSystem.addGravityHole(Point3d(-100.0, 55.0, 0.0), 850);
  mParticuleSystem.addGravityHole(Point3d(-100.0, -55.0, 0.0), 5000);
  
  updateUi();
  mParticuleSystem.start();
  mTimerId = startTimer(kIterationInterval);  
}

//-----------------------------------------------------------------------------
void MainDialog::addGravityHole()
{
	mParticuleSystem.addGravityHole(Point3d(0.0), 0);
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::removeGravityHole()
{
	int r = mpGravityHolesList->currentRow();
  if(r >= 0)
  	mParticuleSystem.removeGravityHole(r);
  updateUi();
}

//-----------------------------------------------------------------------------
void MainDialog::cameraModeChanged(int iId)
{
	if(iId == 0) //2d
  {
    //mpViewer->setCameraMode(Camera::ORTHOGONAL);
    mpViewer->setCameraOrientation(Camera::XY);
  }
  else //3d
  {
    //mpViewer->setCameraMode(Camera::PERSPECTIVE);
    mpViewer->setCameraOrientation(Camera::FREE);
  }
}

//-----------------------------------------------------------------------------
void MainDialog::computingModeChanged(int iCm)
{
  mParticuleSystem.setComputingMode((ParticuleSystem::ComputingMode)iCm);
}

//-----------------------------------------------------------------------------
void MainDialog::decayEnabledClicked(bool iClicked)
{
  mParticuleSystem.setDecayEnabled(iClicked);
}

//-----------------------------------------------------------------------------
void MainDialog::gravityHoleForceChanged(const QString& iText)
{
	int r = mpGravityHolesList->currentRow();
	if(r >= 0)
  {
  	bool a;
    float f = iText.toFloat(&a);
    if(a)
    	mParticuleSystem.setGravityHoleForce(r, f);
  }
}

//-----------------------------------------------------------------------------
void MainDialog::gravityHoleSelected(unsigned int iIndex)
{
  mpGravityHolesList->setCurrentRow(iIndex);
}

//-----------------------------------------------------------------------------
void MainDialog::gravityHoleSelectionChanged(int iRow)
{
	if(iRow >= 0)
  	mpGravityHoleForce->setText(
      QString::number(mParticuleSystem.getGravityHoles()[iRow].mForce));
  else
    mpGravityHoleForce->setText(QString::number(0));
}

//-----------------------------------------------------------------------------
void MainDialog::hidePanel(bool iH)
{ iH ? mpLeftPanel->hide() : mpLeftPanel->show(); }

//-----------------------------------------------------------------------------
void MainDialog::maxInitialLifeChanged(const QString& iText)
{
  bool r = false;
	unsigned int n = iText.toUInt(&r);
  if(r)
  	mParticuleSystem.setMaximumInitialLife(n);
  else
    mParticuleSystem.setMaximumInitialLife(0);
}

//-----------------------------------------------------------------------------
void MainDialog::maxInitialVelocityChanged(const QString& iText)
{
  bool r = false;
	unsigned int n = iText.toUInt(&r);
  if(r)
  	mParticuleSystem.setMaximumInitialVelocity(n);
  else
    mParticuleSystem.setMaximumInitialVelocity(0);
}

//-----------------------------------------------------------------------------
void MainDialog::maxParticulesChanged(const QString& iText)
{
	killTimer(mTimerId);
	bool r = false;
	unsigned int n = iText.toUInt(&r);
  if(r)
  	mParticuleSystem.setNumberOfParticules(n);
  else
    mParticuleSystem.setNumberOfParticules(0);

  mParticuleSystem.initialize();
  mTimerId = startTimer(kIterationInterval);
}

//-----------------------------------------------------------------------------
void MainDialog::timerEvent(QTimerEvent* ipEvent)
{
	mParticuleSystem.iterate();
  mpViewer->repaint();
}

//-----------------------------------------------------------------------------
void MainDialog::updateUi()
{
  mpMaxParticules->setText(QString::number(mParticuleSystem.getNumberOfParticules()));
  mpMaxInitialVelocity->setText(QString::number(mParticuleSystem.getMaximumInitialVelocity()));
  mpMaxInitialLife->setText(QString::number(mParticuleSystem.getMaximumInitialLife()));
  mpDecayEnabled->setChecked(mParticuleSystem.isDecayEnabled());
  
  mpGravityHolesList->clear();
  for(unsigned int i = 0; i < mParticuleSystem.getGravityHoles().size(); ++i)
  {
  	QListWidgetItem* pItem = new QListWidgetItem(mpGravityHolesList);
    pItem->setText(QString::number(i));
  }
}