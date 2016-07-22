/**/

#pragma once

#include "3d/Shader.h"
#include "3d/Texture.h"
#include "3d/Widget3d.h"
#include "math/PlatonicSolid.h"


class Viewer : public realisim::treeD::Widget3d
{
public:
  Viewer(QWidget*);
  ~Viewer();

private:
  virtual void initializeGL();
  //void drawPlatonicSolid(const realisim::math::PlatonicSolid&);
  virtual void paintGL();
  virtual void timerEvent(QTimerEvent*);
};

