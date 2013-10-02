

#ifndef MainDialog_hh
#define MainDialog_hh

#include "3d/Shader.h"
#include "3d/Sprite.h"
#include "3d/Text.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"
#include <QGraphicsScene>
#include <QMainWindow>
#include <QtGui>
#include "utils/SpriteCatalog.h"

class MainDialog;

class Viewer : public realisim::treeD::Widget3d
{
public:
	Viewer( MainDialog&, QWidget*, const QGLWidget* = 0 );
  virtual ~Viewer() {;}
  
  virtual bool isPreviewing() const;
  virtual void setAsPreview(bool);
  
protected:
	enum state{ sNavigation, sEdition, sPreview };
	enum mouseState{ msIdle, msDown, msMove, msDrag };
  enum pickables{ pTopLeftHandle, pTopRightHandle,
  	pBottomLeftHandle, pBottomRightHandle };
  enum colors{ cHover, cSelect };
  
  virtual void drawModeButton(bool = false) const;
  virtual void drawSceneForPicking() const;
  virtual void drawSprites(bool = false) const;
//  virtual QColor getColor( colors ) const;
  virtual void handleDrag();
  virtual void handleSelection();
	virtual void initializeGL();
  virtual void mouseMoveEvent( QMouseEvent* );
  virtual void mousePressEvent( QMouseEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );
	virtual void paintGL();
  
	MainDialog& mMain;
  state mState;
  mouseState mMouseState;
  realisim::math::Point2i mMousePos;
  realisim::math::Vector2i mMouseDelta;
  realisim::math::Vector2i mDragDelta;
  int mHoverId;
  int mSelectedId;
  mutable realisim::treeD::Text mStateButton;
  bool mIsPreviewing;
  realisim::treeD::Shader mShowAlphaShader;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class MainDialog : public QMainWindow
{
	Q_OBJECT
public:
	MainDialog();
	~MainDialog(){};
  
public slots:
            
protected slots:
	void addSpriteClicked();
	void addTextureClicked();
  void closeCatalogClicked();
  void durationChanged(int);
  void frameGridXChanged(int);
  void frameGridYChanged(int);
  void loopingChanged(int);
  void numberOfFramesChanged(int);
	void openCatalogClicked();
  void removeSpriteClicked();
  void removeTextureClicked();
  void save();
  void saveAs();
  void spriteSelectionChanged(int);
  void spriteTokenChanged(QWidget*);
  void tabChanged(int);
  void textureSelectionChanged(int);
  
protected:
	friend class Viewer;

	enum tabType{ ttTexture = 0, ttSprite };
  virtual void timerEvent(QTimerEvent*);
  virtual void updatePreviewerCamera();
  virtual void updateSpriteUi();
  virtual void updateTextureUi();
  virtual void updateUi();
  
  //--- data
	realisim::utils::SpriteCatalog mSpriteCatalog;
  QString mTextureToken;
  QString mSpriteToken;
  QString mSaveFileName;
  int mPreviewTimer;
  
  //--- ui
  Viewer* mpViewer;
  Viewer* mpPreviewer;
  QListWidget* mpTextures;  
  QPushButton* mpAddTexture;
  QPushButton* mpRemoveTexture;

  
  //pour sprite
  QListWidget* mpSprites;
  QPushButton* mpAddSprite;
  QPushButton* mpRemoveSprite;
  QSpinBox* mpDuration;
  QSpinBox* mpFrameGridX;
  QSpinBox* mpFrameGridY;
  QSpinBox* mpNumFrames;
  QCheckBox* mpIsLooping;
};

#endif
