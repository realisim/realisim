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

using namespace RealEdit;

DisplayData::DisplayData() : mCube(0)
{
}

DisplayData::~DisplayData()
{
}

void DisplayData::drawCube() const
{
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT | GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3d(0, 85/255.0, 176/255.0);
  glCallList(mCube);
  glPopAttrib();
}

void DisplayData::initDisplayList()
{
  assert (mCube == 0);
  float hs = 0.5;
  mCube = glGenLists(1);
  glNewList(mCube, GL_COMPILE );
  glBegin(GL_QUADS);
    glNormal3f( 0.0F, 0.0F, 1.0F);
    glVertex3f( hs, hs, hs); glVertex3f(-hs, hs, hs);
    glVertex3f(-hs,-hs, hs); glVertex3f( hs,-hs, hs);

    glNormal3f( 0.0F, 0.0F,-1.0F);
    glVertex3f(-hs,-hs,-hs); glVertex3f(-hs, hs,-hs);
    glVertex3f( hs, hs,-hs); glVertex3f( hs,-hs,-hs);

    glNormal3f( 0.0F, 1.0F, 0.0F);
    glVertex3f( hs, hs, hs); glVertex3f( hs, hs,-hs);
    glVertex3f(-hs, hs,-hs); glVertex3f(-hs, hs, hs);

    glNormal3f( 0.0F,-1.0F, 0.0F);
    glVertex3f(-hs,-hs,-hs); glVertex3f( hs,-hs,-hs);
    glVertex3f( hs,-hs, hs); glVertex3f(-hs,-hs, hs);

    glNormal3f( 1.0F, 0.0F, 0.0F);
    glVertex3f( hs, hs, hs); glVertex3f( hs,-hs, hs);
    glVertex3f( hs,-hs,-hs); glVertex3f( hs, hs,-hs);

    glNormal3f(-1.0F, 0.0F, 0.0F);
    glVertex3f(-hs,-hs,-hs); glVertex3f(-hs,-hs, hs);
    glVertex3f(-hs, hs, hs); glVertex3f(-hs, hs,-hs);
  glEnd();
  glEndList();
}
