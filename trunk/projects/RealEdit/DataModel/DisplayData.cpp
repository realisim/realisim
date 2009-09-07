/*
 */

#include "DisplayData.h"
#include <qgl.h>

using namespace realisim;
using namespace realEdit;

DisplayData::DisplayData() : mAxis(), mCube()
{}

DisplayData::~DisplayData()
{}

void DisplayData::drawAxis() const
{
  glCallList(mAxis.getDisplayList());
}

void DisplayData::drawCube() const
{
  glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glCallList(mCube.getDisplayList());
  glPopAttrib();
}
