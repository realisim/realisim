/*
 *  MainWindow.h
 */

#ifndef MainDialog_hh
#define MainDialog_hh

#include <QMainWindow>
#include "3d/Particules.h"
#include "3d/Widget3d.h"
#include "3d/SpriteCatalog.h"

class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSlider;
class QString;
class QTimerEvent;
class Viewer;
namespace realisim { namespace treeD { class Texture; } }

//------------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
	int getNumberOfParticuleSystems() const;
  realisim::treeD::Particules& getParticuleSystem(int);
  
protected slots:
	virtual void addClicked();
  virtual void colorClicked();
  virtual void lifeLowerRangeChanged( const QString& );
  virtual void lifeUpperRangeChanged( const QString& );
  virtual void numberOfParticulesChanged( const QString& );
  virtual void radiusChanged( int );
  virtual void rateChanged( int );
  virtual void removeClicked();
  virtual void sizeChanged( int );
  virtual void typeChanged( int );
  virtual void updateUi();
  virtual void velocityLowerRangeChanged( const QString& );
  virtual void velocityUpperRangeChanged( const QString& );
                
protected:
	friend class Viewer;
  
	virtual void createUi();
  virtual realisim::treeD::Particules& getSelectedSource();
  
  Viewer* mpViewer;
  std::vector< realisim::treeD::Particules > mParticules;
  static realisim::treeD::Particules mDummyParticules;
  int mSelectionId;
  
  QListWidget* mpParticules;
  QPushButton* mpColorButton;
  QLineEdit* mpNumberOfParticules;
  QComboBox* mpType;
  QLineEdit* mpLifeLowerBound;
  QLineEdit* mpLifeUpperBound;
  QLineEdit* mpVelocityLowerBound;
  QLineEdit* mpVelocityUpperBound;
  QSlider* mpRadiusSlider;
  QSlider* mpRateSlider;
  QSlider* mpSizeSlider;
};

//------------------------------------------------------------------------------
class Viewer : public realisim::treeD::Widget3d
{
public:
	Viewer( QWidget*, MainDialog& );
  virtual ~Viewer();
  
protected:
  virtual void draw();
  virtual void drawSceneForPicking();
	virtual void initializeGL();
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void timerEvent( QTimerEvent* );
  
	MainDialog& mMainDialog;
  int mTimerId;
  realisim::treeD::SpriteCatalog mSpriteCatalog;
};


#endif
