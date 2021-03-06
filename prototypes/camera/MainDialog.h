
#ifndef MainDialog_hh
#define MainDialog_hh

#include <QMainWindow>
#include "3d/Widget3d.h"

class Viewer;
class QComboBox;
class QLabel;
class QLineEdit;

//------------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
protected slots:
	void cameraChanged(int);
	void cameraControlChanged(int);
  void cameraProjectionChanged(int);
  void zoomChanged();
  void updateUi();
                
protected:
	void createUi();
  
  QComboBox* mpCameras;
  QComboBox* mpCameraProjection;
  QComboBox* mpCameraControl;
  QLineEdit* mpZoom;
  QLabel* mpCameraInfo;
  Viewer* mpViewer;
  std::vector< realisim::treeD::Camera > mCameras;
};

//------------------------------------------------------------------------------
class Viewer : public realisim::treeD::Widget3d
{
public:
	Viewer( QWidget* );
  virtual ~Viewer();
  
protected:
  virtual void draw();
	virtual void initializeGL();
  virtual void keyPressEvent( QKeyEvent* );
//  virtual void keyReleaseEvent( QKeyEvent* );
//  virtual void mouseMoveEvent( QMouseEvent* e );
//  virtual void mousePressEvent( QMouseEvent* e );
//  virtual void mouseReleaseEvent( QMouseEvent* e );
  virtual void timerEvent( QTimerEvent* );
  
  std::vector< realisim::math::Point3d > mObjectPositions;
};

#endif
