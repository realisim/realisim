#ifndef MainWindow__hh
#define MainWindow__hh

#include "Engine.h"
class QFrame;
class QKeyEvent;
class QLineEdit;
#include <QMainWindow>
#include "Viewer.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow(){};
       
  bool isDebugging() const {return mIsDebugging;}
  void setAsDebugging(bool);
    
protected slots:
  virtual void areaToRender0RadiusChanged(const QString&);
  virtual void areaToRender1RadiusChanged(const QString&);
  virtual void load();
  virtual void save();
  virtual void spaceRadiusChanged(const QString&);
  virtual void thresholdToRenderCubeMapChanged(const QString&);
  
protected:
  virtual void keyPressEvent(QKeyEvent*);  
  virtual void updateUi();

	QFrame* mpDebugFrame;
  QLineEdit* mpNumberOfAstronomicalBodies;
  QLineEdit* mpRadiusOfGeneration;
  QLineEdit* mpSpaceRadius;
  QLineEdit* mpAreaToRender0Radius;
  QLineEdit* mpAreaToRender1Radius;
  QLineEdit* mpThresholdToRenderCubeMap;
  
  SpaceTime::Viewer* mpViewer;
	SpaceTime::Engine mEngine;
  bool mIsDebugging;
};

#endif
