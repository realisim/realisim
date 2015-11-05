#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include "mainWindow.h"
#include "QString"
#include <QtGui>
#include "coreLibrary, glSceneGraph.h"
#include "utilities3d.h"   

using namespace sharkTank;
using namespace std;
using namespace resonant;
   using namespace coreLibrary;

namespace
{
   point3 toPoint3( const vector3& v)
   { return point3(v.dx(), v.dy(), v.dz()); }
}
//-----------------------------------------------------------------------------
//--- myViewer
//-----------------------------------------------------------------------------
myViewer::myViewer( engine& iEngine, QWidget* ipParent ) : glViewer(ipParent), 
   engine::listener(),
   mEngine( iEngine )
{
   mEngine.addListener(this);
}
//-----------------------------------------------------------------------------
void myViewer::draw()
{
   glViewer::draw();

   mEngine.getSceneGraph().render();

   /*glDisable(GL_LIGHTING);
   glPushMatrix();
   glScaled( 10, 10, 10 );
   utilities::drawAxis();
   glPopMatrix();
   glEnable(GL_LIGHTING);*/
}
//-----------------------------------------------------------------------------
void myViewer::initializeGL()
{
   glViewer::initializeGL();
   // background blanc
   float c = 0.2f;
   glClearColor(c, c, c, 0.0f);
}
//-----------------------------------------------------------------------------
void myViewer::received(sharkTank::engine::message iM)
{
   switch (iM)
   {
   case engine::mFrameReady: update(); break;
   default: break;
   }
}
//-----------------------------------------------------------------------------
//--- MainWindow
//-----------------------------------------------------------------------------

void printMatrix( matrix4 m )
{
   for(int i = 0; i < 4; ++i)
   {
      for(int j = 0; j < 4; ++j)
      { printf("%.2f, ", m(i, j)); }
      printf("\n");
   }
}
//-----------------------------------------------------------------------------
mainWindow::mainWindow( QWidget* ipParent /*=0*/ ) : QMainWindow( ipParent )
{
   createUi();
   updateUi();
}
mainWindow::~mainWindow()
{}
//-----------------------------------------------------------------------------
void mainWindow::addShark()
{ mEngine.addShark(); }
//-----------------------------------------------------------------------------
void mainWindow::addFood()
{ mEngine.addFood(); }
//-----------------------------------------------------------------------------
void mainWindow::createUi()
{
   resize( 800, 600 );
   QFrame* pMainframe = new QFrame(this);
   setCentralWidget(pMainframe);

   QHBoxLayout* pMainLyt = new QHBoxLayout( pMainframe );
   {
      QVBoxLayout* pLeftLyt = new QVBoxLayout();
      {
         //camera movement increment
         QHBoxLayout* pCameraLyt = new QHBoxLayout();
         pCameraLyt->setMargin(2); pCameraLyt->setSpacing(2);
         {
            QLabel* l = new QLabel("camera increment:", pMainframe);
            QSlider* pSlider = new QSlider(Qt::Horizontal, pMainframe);
            pSlider->setMinimum(0);
            pSlider->setMaximum(100);
            connect( pSlider, SIGNAL(valueChanged(int)), this,
               SLOT(cameraIncrementChanged(int)) );

            pCameraLyt->addWidget(l);
            pCameraLyt->addWidget(pSlider);
         }

         //fish minimal separation distance
         QHBoxLayout* pFishMinSeparation = new QHBoxLayout();
         {
            QLabel* l = new QLabel("fish min separation:", pMainframe);
            mpFishMinSeparationSpinBox = new QSpinBox(pMainframe);
            mpFishMinSeparationSpinBox->setMinimum( 1 );
            mpFishMinSeparationSpinBox->setMaximum( 100 );
            connect( mpFishMinSeparationSpinBox, SIGNAL(valueChanged(int)),
               this, SLOT(fishMinSeparationChanged(int)) );

            pFishMinSeparation->addWidget(l);
            pFishMinSeparation->addWidget(mpFishMinSeparationSpinBox);
         }
         //fish min number of neighboor to flock with
         QHBoxLayout* pFishMinFlockNeighbour = new QHBoxLayout();
         {
            QLabel* l = new QLabel("fish min flock neighbour:", pMainframe);
            mpFishMinFlockSpinBox = new QSpinBox(pMainframe);
            mpFishMinFlockSpinBox->setMinimum( 1 );
            mpFishMinFlockSpinBox->setMaximum( 100 );
            connect( mpFishMinFlockSpinBox, SIGNAL(valueChanged(int)),
               this, SLOT(fishMinFlockNeighbourChanged(int)) );

            pFishMinFlockNeighbour->addWidget(l);
            pFishMinFlockNeighbour->addWidget(mpFishMinFlockSpinBox);
         }
         //fish grouping force factor
         QHBoxLayout* pFishGroupingForceFactorLyt = new QHBoxLayout();
         {
            QLabel* l = new QLabel("fish grp force factor:", pMainframe);
            mpFishGrpForceFactor = new QLineEdit(pMainframe);            
            connect( mpFishGrpForceFactor, SIGNAL(textChanged(const QString&)),
               this, SLOT(fishGroupingForceFactorChanged(const QString&)) );

            pFishGroupingForceFactorLyt->addWidget(l);
            pFishGroupingForceFactorLyt->addWidget(mpFishGrpForceFactor);
         }
         //fish separation force factor
         QHBoxLayout* pFishSeparationForceFactorLyt = new QHBoxLayout();
         {
            QLabel* l = new QLabel("fish separation force factor:", pMainframe);
            mpFishSepForceFactor = new QLineEdit(pMainframe);            
            connect( mpFishSepForceFactor, SIGNAL(textChanged(const QString&)),
               this, SLOT(fishSeparationForceFactorChanged(const QString&)) );

            pFishSeparationForceFactorLyt->addWidget(l);
            pFishSeparationForceFactorLyt->addWidget(mpFishSepForceFactor);
         }
         //fish steering force factor
         QHBoxLayout* pFishSteeringForceFactorLyt = new QHBoxLayout();
         {
            QLabel* l = new QLabel("fish steering force factor:", pMainframe);
            mpFishSteeringForceFactor = new QLineEdit(pMainframe);            
            connect( mpFishSteeringForceFactor, SIGNAL(textChanged(const QString&)),
               this, SLOT(fishSteeringForceFactorChanged(const QString&)) );

            pFishSteeringForceFactorLyt->addWidget(l);
            pFishSteeringForceFactorLyt->addWidget(mpFishSteeringForceFactor);
         }


         //addShark
         QHBoxLayout* pActionLyt = new QHBoxLayout();
         {
            QPushButton* pAddShark = new QPushButton("add shark", pMainframe);
            connect(pAddShark, SIGNAL(clicked()), this, SLOT(addShark()));

            //nudge button
            QPushButton* pNudge = new QPushButton("nudge", pMainframe);
            connect(pNudge, SIGNAL(clicked()), this, SLOT(nudgeFish()));

            //add food button
            QPushButton* pAddFood = new QPushButton("add food", pMainframe);
            connect(pAddFood, SIGNAL(clicked()), this, SLOT(addFood()));

            pActionLyt->addWidget(pAddShark);
            pActionLyt->addWidget(pNudge);
            pActionLyt->addWidget(pAddFood);
         }
         

         //restart
         QPushButton* pRestart = new QPushButton("restart", pMainframe);
         connect(pRestart, SIGNAL(clicked()), this, SLOT(restart()));

         pLeftLyt->addLayout(pCameraLyt);
         pLeftLyt->addLayout(pFishMinSeparation);
         pLeftLyt->addLayout(pFishMinFlockNeighbour);
         pLeftLyt->addLayout(pFishGroupingForceFactorLyt);
         pLeftLyt->addLayout(pFishSeparationForceFactorLyt);
         pLeftLyt->addLayout(pFishSteeringForceFactorLyt);
         
         
         pLeftLyt->addStretch(1);
         pLeftLyt->addLayout(pActionLyt);
         pLeftLyt->addWidget(pRestart);
         
      }

      QVBoxLayout* pRightLyt = new QVBoxLayout();
      {
         mpMyViewer = new myViewer( mEngine, pMainframe );      
         glCamera c = mpMyViewer->getCamera();
         c.set( point3(50, 0, 0), point3(0.0, 0.0, 0.0), vector3(0,0,1) );      
         c.setPerspectiveProjection( 65 );
         mpMyViewer->setCamera(c);
         mpMyViewer->setCameraControl( glViewer::ccFreeFlight );
         mpMyViewer->setCameraMoveIncrement( 1 );

         pRightLyt ->addWidget( mpMyViewer, 5 );
      }

      pMainLyt->addLayout( pLeftLyt, 1 );
      pMainLyt->addLayout( pRightLyt, 3 );
   }

   updateUi();
}
//-----------------------------------------------------------------------------
void mainWindow::cameraIncrementChanged(int iPercentage)
{
   const double kMin = 0.1;
   const double kMax = 50;
   mpMyViewer->setCameraMoveIncrement( kMin + (kMax - kMin) * iPercentage/100.0 );
}
//-----------------------------------------------------------------------------
void mainWindow::fishGroupingForceFactorChanged(const QString& iText)
{
   bool ok = false;
   double d = iText.toDouble(&ok);
   if(ok){ mEngine.setGroupingForceFactor(d); }
}
//-----------------------------------------------------------------------------
void mainWindow::fishMinSeparationChanged(int iV)
{
   mEngine.setFishMinimalSeparationDistance(iV);
   updateUi();
}
//-----------------------------------------------------------------------------
void mainWindow::fishMinFlockNeighbourChanged(int iV)
{
   mEngine.setFishMinimalNumberOfNeighbourToFlock(iV);
   updateUi();
}
//-----------------------------------------------------------------------------
void mainWindow::fishSeparationForceFactorChanged(const QString& iText)
{
   bool ok = false;
   double d = iText.toDouble(&ok);
   if(ok){ mEngine.setSeparationForceFactor(d); }
}
//-----------------------------------------------------------------------------
void mainWindow::fishSteeringForceFactorChanged(const QString& iText)
{
   bool ok = false;
   double d = iText.toDouble(&ok);
   if(ok){ mEngine.setSteeringForceFactor(d); }
}
//-----------------------------------------------------------------------------
void mainWindow::nudgeFish()
{ mEngine.nudgeFish(); }
//-----------------------------------------------------------------------------
void mainWindow::restart()
{
   mEngine.stop();
   mEngine.start();
}
//-----------------------------------------------------------------------------
void mainWindow::updateUi()
{
   mpMyViewer->update();

   mpFishMinSeparationSpinBox->setValue( mEngine.getFishMinimalSeparationDistance() );
   mpFishMinFlockSpinBox->setValue( mEngine.getFishMinimalNumberOfNeighbourToFlock() );
   mpFishGrpForceFactor->setText( QString::number(mEngine.getGroupingForceFactor(), 'g', 1) );
   mpFishSepForceFactor->setText( QString::number(mEngine.getSeparationForceFactor(), 'g', 1) );
   mpFishSteeringForceFactor->setText( QString::number(mEngine.getSteeringForceFactor(), 'g', 1) );   
   
   update();
}
//-----------------------------------------------------------------------------
void mainWindow::resizeEvent(QResizeEvent* ipE)
{   
   mEngine.start();
   mEngine.setViewerWindowSize( ipE->size() );
}

////-----------------------------------------------------------------------------
//void mainWindow::
//{}

