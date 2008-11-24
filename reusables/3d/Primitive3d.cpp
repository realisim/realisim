/*
 *  Primitives3d.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 22/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Primitive3d.h"

#include <qgl.h>

using namespace Realisim;
using namespace Realisim::Primitive3d;

namespace
{
  const int kInvalidDisplayList = -1;
}

Primitive3dBase::Primitive3dBase()
{
}

Primitive3dBase::~Primitive3dBase()
{
}

//--------------------------Cube------------------------------------------------
int Cube::mDisplayList = kInvalidDisplayList;

Cube::Cube() : Primitive3dBase()
{
}

Cube::~Cube()
{
}

void
Cube::draw( bool iPickingDraw /* = false */ ) const
{
  initDisplayList();
  
  glCallList( mDisplayList );
}

void
Cube::initDisplayList() const
{
  if( mDisplayList != kInvalidDisplayList )
  {
    return;
  }
  
  float hs = 0.5;
  
  mDisplayList = glGenLists(1);
  glNewList(mDisplayList, GL_COMPILE );
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

