/*
 *  EditionData.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 19/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DisplayData.h"
#include <qgl.h>

using namespace Realisim;
using namespace RealEdit;

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
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT | GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3d(0, 85/255.0, 176/255.0);
  glCallList(mCube.getDisplayList());
  glPopAttrib();
}
