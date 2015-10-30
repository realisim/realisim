#ifndef resonant_mainWindow_h
#define resonant_mainWindow_h

#include "engine.h"
#include <QMainWindow>
#include "QSpinBox"
#include "coreLibrary, glViewer.h"

class myViewer : public resonant::coreLibrary::glViewer, 
  public sharkTank::engine::listener
{
public:
   myViewer( sharkTank::engine&, QWidget* ipParent = 0 );

   virtual void draw();

protected:
   virtual void initializeGL();
   virtual void received(sharkTank::engine::message);

   sharkTank::engine& mEngine;
};

class mainWindow : public QMainWindow
{
   Q_OBJECT
public :
   mainWindow( QWidget* ipParent = 0 );
   virtual ~mainWindow();

protected slots:
   void addFood();
   void addShark();
   void cameraIncrementChanged(int);
   void fishGroupingForceFactorChanged(const QString&);
   void fishMinSeparationChanged(int);
   void fishMinFlockNeighbourChanged(int);
   void fishSeparationForceFactorChanged(const QString&);
   void fishSteeringForceFactorChanged(const QString&);
   
   void nudgeFish();
   void restart();

protected:
   void createUi();
   virtual void resizeEvent(QResizeEvent*);
   virtual void updateUi();

   myViewer* mpMyViewer;
   sharkTank::engine mEngine;

   //---UI
   QSpinBox* mpFishMinSeparationSpinBox;
   QSpinBox* mpFishMinFlockSpinBox;
   QLineEdit* mpFishGrpForceFactor;
   QLineEdit* mpFishSepForceFactor;
   QLineEdit* mpFishSteeringForceFactor;   
};


#endif 
