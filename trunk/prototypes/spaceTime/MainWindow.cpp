
#include "3d/Camera.h"
#include "Engine.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/MathUtils.h"
#include "MainWindow.h"
#include <qapplication.h>
#include <QFile.h>
#include <QFileDialog.h>
#include <QFrame>
#include <QKeyEvent>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <QIntValidator>
#include <QPushButton>
#include <time.h>
#include "utils/utilities.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
using namespace SpaceTime;


//-----------------------------------------------------------------------------
//--- MainWindow
//-----------------------------------------------------------------------------
MainWindow::MainWindow() : QMainWindow(),
  mpDebugFrame(0),
  mpNumberOfAstronomicalBodies(0),
  mpRadiusOfGeneration(0),
  mpSpaceRadius(0),
  mpAreaToRender0Radius(0),
  mpAreaToRender1Radius(0),
  mpThresholdToRenderCubeMap(0),
  mpViewer(0),
  mEngine(),
  mIsDebugging(false)
{
  resize(800, 600);

  //initialise le layout
  QFrame* pMainFrame = new QFrame(this);
  setCentralWidget(pMainFrame);
  
  QHBoxLayout* pHlyt = new QHBoxLayout(pMainFrame);
  pHlyt->setSpacing(0);
  pHlyt->setMargin(0);  
  
  //--- le paneau de debug
  mpDebugFrame = new QFrame(pMainFrame);
  mpDebugFrame->setFixedWidth(250);
  QVBoxLayout* pMainDebugLyt = new QVBoxLayout(mpDebugFrame);
  pMainDebugLyt->setMargin(1);
  pMainDebugLyt->setSpacing(1);
  
  //--- Relié a l'engin
  //ligne 1 du paneau de debug
  QHBoxLayout* pLine1Lyt = new QHBoxLayout(mpDebugFrame);
  QLabel* pNumAbLabel = new QLabel("num bodies:", mpDebugFrame);
  mpNumberOfAstronomicalBodies = new QLineEdit("500", mpDebugFrame);
  QIntValidator* pValNumAb = new QIntValidator(0,
    std::numeric_limits<int>::max(), mpNumberOfAstronomicalBodies);
  mpNumberOfAstronomicalBodies->setValidator(pValNumAb);
  pLine1Lyt->addWidget(pNumAbLabel);
  pLine1Lyt->addWidget(mpNumberOfAstronomicalBodies);
  
  //ligne 2 du paneau de debug
  QHBoxLayout* pLine2Lyt = new QHBoxLayout(mpDebugFrame);
  QLabel* pRadiusOfGen = new QLabel("radius of generation:", mpDebugFrame);
  mpRadiusOfGeneration = new QLineEdit("100", mpDebugFrame);
  QIntValidator* pValRadiusOfGen = new QIntValidator(0,
    std::numeric_limits<int>::max(), mpRadiusOfGeneration);
  mpRadiusOfGeneration->setValidator(pValRadiusOfGen);
  pLine2Lyt->addWidget(pRadiusOfGen);
  pLine2Lyt->addWidget(mpRadiusOfGeneration);
  
  //ligne 3 du paneau de debug
  QHBoxLayout* pLine3Lyt = new QHBoxLayout(mpDebugFrame);
  QLabel* pSpaceRadius = new QLabel("radius of space:", mpDebugFrame);
  mpSpaceRadius = new QLineEdit("100000", mpDebugFrame);
  connect(mpSpaceRadius, SIGNAL(textChanged(const QString&)), 
    this, SLOT(spaceRadiusChanged(const QString&)));
  QIntValidator* pValSpaceRadius = new QIntValidator(0,
    std::numeric_limits<int>::max(), mpSpaceRadius);
  mpSpaceRadius->setValidator(pValSpaceRadius);
  pLine3Lyt->addWidget(pSpaceRadius);
  pLine3Lyt->addWidget(mpSpaceRadius);
  
  //--- Relié au visualiseur
  //ligne 1 du paneau de debug pour relié au visualiseur
  QHBoxLayout* pLineB1Lyt = new QHBoxLayout(mpDebugFrame);
  QLabel* pCubeMapRadius = new QLabel("area0 radius:", mpDebugFrame);
  mpAreaToRender0Radius = new QLineEdit(mpDebugFrame);
  connect(mpAreaToRender0Radius, SIGNAL(textChanged(const QString&)),
    this, SLOT(areaToRender0RadiusChanged(const QString&)) );
  QDoubleValidator* pValCubeMap0Radius = new QDoubleValidator(0.0,
    std::numeric_limits<double>::max(), 2, mpAreaToRender0Radius);
  mpAreaToRender0Radius->setValidator(pValCubeMap0Radius);
  pLineB1Lyt->addWidget(pCubeMapRadius);
  pLineB1Lyt->addWidget(mpAreaToRender0Radius);
  
  //ligne 2 du paneau de debug pour relié au visualiseur
  QHBoxLayout* pLineB2Lyt = new QHBoxLayout(mpDebugFrame);
  QLabel* pCubeMap1Radius = new QLabel("area1 radius:", mpDebugFrame);
  mpAreaToRender1Radius = new QLineEdit(mpDebugFrame);
  connect(mpAreaToRender1Radius, SIGNAL(textChanged(const QString&)),
    this, SLOT(areaToRender1RadiusChanged(const QString&)) );
  QDoubleValidator* pValCubeMap1Radius = new QDoubleValidator(0.0,
    std::numeric_limits<double>::max(), 2, mpAreaToRender1Radius);
  mpAreaToRender1Radius->setValidator(pValCubeMap1Radius);
  pLineB2Lyt->addWidget(pCubeMap1Radius);
  pLineB2Lyt->addWidget(mpAreaToRender1Radius);
  
  //ligne 3 du paneau de debug pour relié au visualiseur
  QHBoxLayout* pLineB3Lyt = new QHBoxLayout(mpDebugFrame);
  QLabel* pTresholdToRender = new QLabel("threshold to render:", mpDebugFrame);
  mpThresholdToRenderCubeMap = new QLineEdit(mpDebugFrame);
  connect(mpThresholdToRenderCubeMap, SIGNAL(textChanged(const QString&)),
    this, SLOT(thresholdToRenderCubeMapChanged(const QString&)) );
  QDoubleValidator* pValThresholdToRender = new QDoubleValidator(0.0,
    std::numeric_limits<double>::max(), 2, mpThresholdToRenderCubeMap);
  mpThresholdToRenderCubeMap->setValidator(pValThresholdToRender);
  pLineB3Lyt->addWidget(pTresholdToRender);
  pLineB3Lyt->addWidget(mpThresholdToRenderCubeMap);
  
  //--- section de sauvegarde
  QHBoxLayout* pLineC1Lyt = new QHBoxLayout(mpDebugFrame);
  QPushButton* pLoad = new QPushButton("load...", mpDebugFrame);
  connect(pLoad, SIGNAL(clicked()), this, SLOT(load()));
  QPushButton* pSave = new QPushButton("save...", mpDebugFrame);
  connect(pSave, SIGNAL(clicked()), this, SLOT(save()));
  pLineC1Lyt->addStretch(1);
  pLineC1Lyt->addWidget(pLoad);
  pLineC1Lyt->addWidget(pSave);
  
  pMainDebugLyt->addLayout(pLine1Lyt);
  pMainDebugLyt->addLayout(pLine2Lyt);
  pMainDebugLyt->addLayout(pLine3Lyt);
  pMainDebugLyt->addWidget(new QLabel("----", mpDebugFrame));
  pMainDebugLyt->addLayout(pLineB1Lyt);
  pMainDebugLyt->addLayout(pLineB2Lyt);
  pMainDebugLyt->addLayout(pLineB3Lyt);
  pMainDebugLyt->addLayout(pLineC1Lyt);
  pMainDebugLyt->addStretch(1);
  
  //--- le viewer
  mpViewer = new Viewer(pMainFrame, mEngine);
  mpViewer->initialize();
  
  pHlyt->addWidget(mpDebugFrame);
  pHlyt->addWidget(mpViewer, 1);  
  
  //on enregistre le viewer comme client de l'engin
  mEngine.registerClient(mpViewer);
  mEngine.goToState(Engine::sPlaying);
    
  setAsDebugging(true);  
  updateUi();
}

//-----------------------------------------------------------------------------
void MainWindow::areaToRender0RadiusChanged(const QString& iT)
{
	double r = iT.toDouble(0);
	mpViewer->setAreaToRenderRadius(0, r);
}

//-----------------------------------------------------------------------------
void MainWindow::areaToRender1RadiusChanged(const QString& iT)
{
	double r = iT.toDouble(0);
	mpViewer->setAreaToRenderRadius(1, r);
}

//-----------------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent* ipE)
{
	switch (ipE->key()) 
  {
  	case Qt::Key_Escape:
    	if(mEngine.getState() == Engine::sPlaying)
      	mEngine.goToState(Engine::sPaused);
      else
      {
      	mpViewer->centerMouse();
				mEngine.goToState(Engine::sPlaying);
      }
    break;
    case Qt::Key_G:
    {
    	mEngine.goToState(Engine::sSimulating);
      update();
    }
    break;
    case Qt::Key_M:
    {
    	setAsDebugging(!isDebugging());
      update();
    }
    break;
    case Qt::Key_N:
    {
    	long long n = mpNumberOfAstronomicalBodies->text().toLongLong(0);
      long long r = mpRadiusOfGeneration->text().toLongLong(0);
      mEngine.setRadiusOfGeneration(r);
      mEngine.generateAstronomicalBodies(n);
      mpViewer->invalidateCubeMapRender();
      update();
    }
    break;
    case Qt::Key_P:
    {
    	mEngine.fonctionBidon();
      update();
    }
    break;
    case Qt::Key_T:
    {
      long long r = mpRadiusOfGeneration->text().toLongLong(0);
      mEngine.setRadiusOfGeneration(r);
      mEngine.generateTestBodies1();
      mpViewer->invalidateCubeMapRender();
      update();
    }
    break;
    case Qt::Key_Y:
    {
      long long n = mpNumberOfAstronomicalBodies->text().toLongLong(0);
      long long r = mpRadiusOfGeneration->text().toLongLong(0);
      mEngine.setRadiusOfGeneration(r);
      mEngine.generateTestBodies2(n);
      mpViewer->invalidateCubeMapRender();
      update();
    }
    break;
case Qt::Key_Plus:
{
  mEngine.step();
  update();
}
break;
  	default: ipE->setAccepted(false); break;
  }
}

//-----------------------------------------------------------------------------
void MainWindow::load()
{
  QString file = QFileDialog::getOpenFileName(this, tr("load"),
    "./", "*.sav");
  if(file.isNull())
  	return;

  QByteArray ba = utils::fromFile(file);
  mEngine.fromBinary(ba);
  mpViewer->invalidateCubeMapRender();
  mpViewer->update();
  updateUi();
}

//-----------------------------------------------------------------------------
void MainWindow::save()
{
  QString file = QFileDialog::getSaveFileName(this, tr("save"),
  "./untitled.sav",
  tr("*.sav"));
  QByteArray ba = mEngine.toBinary();
  utils::toFile(file, ba);
}

//-----------------------------------------------------------------------------
void MainWindow::setAsDebugging(bool iD)
{
	mIsDebugging = iD;
  mpDebugFrame->setShown(iD);
	mEngine.setAsDebugging(iD);
  mpViewer->setAsDebugging(iD);
}

//-----------------------------------------------------------------------------
void MainWindow::spaceRadiusChanged(const QString& iT)
{ mEngine.setSpaceRadius(iT.toLongLong(0)); }

//-----------------------------------------------------------------------------
void MainWindow::thresholdToRenderCubeMapChanged(const QString& iT)
{ mpViewer->setThresholdToRenderCubeMap( iT.toDouble(0) ); }

//-----------------------------------------------------------------------------
void MainWindow::updateUi()
{
	//pour l'engin
	mpSpaceRadius->setText( QString::number(mEngine.getSpaceRadius()) );
  
  //pour le viewer
	mpThresholdToRenderCubeMap->setText(
    QString::number(mpViewer->getThresholdToRenderCubeMap()) );
  mpAreaToRender0Radius->setText(
    QString::number(mpViewer->getAreaToRenderRadius(0)) );
  mpAreaToRender1Radius->setText(
    QString::number(mpViewer->getAreaToRenderRadius(1)) );
}
