/*
 *  MainWindow.h
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 08/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Viewer_hh
#define Viewer_hh

#include "3d/FrameBufferObject.h"
#include "3d/Shader.h"
#include "3d/Sprite.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"
#include "math/PlatonicSolid.h"
#include "qobject.h"
class QMouseEvent;
namespace realisim { namespace treeD { class Texture; } }
namespace realisim { namespace openCL { class ParticuleSystem; } }

class Viewer : public realisim::treeD::Widget3d
{
  Q_OBJECT
public:
  Viewer(QWidget*);
  virtual ~Viewer();
  
  const realisim::openCL::ParticuleSystem* getParticuleSystem() const {return mpParticuleSystem;}
  virtual void setParticuleSystem(realisim::openCL::ParticuleSystem* iP) {mpParticuleSystem = iP;}

signals:
	void gravityHoleSelected(unsigned int);
  void hidePanel(bool);
  
private:
	enum ButtonState
  { bsMinus, bsPlus, bsMinusHover, bsPlusHover };

  virtual void initializeGL();
  virtual void drawEmitter(bool = false) const;
  virtual void drawScene() const;
  virtual void drawParticules(bool = false) const;
  virtual void drawGravityHoles(bool = false) const;
  virtual void drawSceneForPicking() const;
  virtual void handlePlusMinusState();
  virtual void mouseDoubleClickEvent(QMouseEvent*);
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
  virtual void paintGL();
  virtual void resizeGL(int, int);
  
  realisim::math::PlatonicSolid mPs;
  realisim::treeD::FrameBufferObject mFbo;
  realisim::treeD::Texture mParticuleTexture;
  realisim::treeD::Texture mGravityHoleTexture;
  
  realisim::openCL::ParticuleSystem* mpParticuleSystem;
  unsigned int mSelectedId;
  int mMousePosX;
  int mMousePosY;
  
  realisim::treeD::Sprite mPlusMinus;
  ButtonState mButtonState;
};

#endif
