/*
 *  RealEdit3d.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 17/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "RealEdit3d.h"

using namespace RealEdit;

RealEdit3d::RealEdit3d( QWidget* ipParent, 
                        const QGLWidget* iSharedWidget, 
                        const EditionData& iEditionData ) : 
Widget3d(ipParent, iSharedWidget),
mEditionData( iEditionData )
{
}

void
RealEdit3d::paintGL()
{
  Widget3d::paintGL();
  
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