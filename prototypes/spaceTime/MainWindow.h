#ifndef MainWindow__hh
#define MainWindow__hh

#include "Engine.h"
class QKeyEvent;
#include <QMainWindow>
#include "Viewer.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow(){};
       
  bool isDebuggin() const {return mIsDebugging;}
  void setAsDebugging(bool);
    
protected:
  virtual void keyPressEvent(QKeyEvent*);

  SpaceTime::Viewer* mpViewer;
	SpaceTime::Engine mEngine;
  bool mIsDebugging;
};

#endif
