/*
 *  Primitives.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 10/05/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Primitives.h"

using namespace Realisim;

Primitives::Primitives() :
  mDisplayList(0),
  mOrientationFlag(oScene),
  mPosition(0.0),
  mPositionFlag(pScene),
  mZoomFlag(zScene)
{}

//void Primitives::draw() const
//{
//}

//------------------------------------------------------------------------------
GLuint Primitives::getDisplayList() const
{
  if(mDisplayList == 0)
  {
    mDisplayList = glGenLists(1);
    glNewList(mDisplayList, GL_COMPILE );
    draw();
    glEndList();
  }
  return mDisplayList;
}

//------------------------------------------------------------------------------
void Axis::draw() const
{
  glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glLineWidth(2);
  glBegin(GL_LINES);
  //x
  glColor3d(1.0, 0.0, 0.0);
  glVertex3d(0.0, 0.0, 0.0); glVertex3d(1.0, 0.0, 0.0);
  //y
  glColor3d(0.0, 1.0, 0.0);
  glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 1.0, 0.0);
  //z
  glColor3d(0.0, 0.0, 1.0);
  glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 0.0, 1.0);
  glEnd();
  glPopAttrib();
}

//------------------------------------------------------------------------------
void Cube::draw() const
{
  float hs = 0.5;
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
}
