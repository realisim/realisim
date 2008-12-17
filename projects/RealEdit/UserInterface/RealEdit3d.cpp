/*
 *  RealEdit3d.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 17/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "RealEdit3d.h"

#include "DataModel.h"
#include "EditionData.h"
#include "ObjectNode.h"

using namespace Realisim;
using namespace RealEdit;

namespace
{
  const unsigned int kInvalidDisplayList = -1;
}

int RealEdit3d::mCube = kInvalidDisplayList;

RealEdit3d::RealEdit3d( QWidget* ipParent, 
                        const QGLWidget* iSharedWidget, 
                        const EditionData& iEditionData ) : 
Widget3d(ipParent, iSharedWidget),
mEditionData( iEditionData )
{
}

RealEdit3d::~RealEdit3d()
{
}

void
RealEdit3d::paintGL()
{
  Widget3d::paintGL();
  
  drawScene( mEditionData.getScene().getObjectNode() );
}

void
RealEdit3d::drawScene( const RealEdit::ObjectNode* ipObjectNode )
{
  const RealEditModel* pModel = ipObjectNode->getModel();
  
  glPushMatrix();
  {
    //appliquer la transfo du noeud
  
    //dessiner les points du modele
    for( unsigned int i = 0; i < pModel->getPointCount(); ++i )
    {
      glPushMatrix();
      {
        const RealEditPoint* pPoint = pModel->getPoint(i);
        glTranslated( pPoint->getX(), pPoint->getY(), pPoint->getZ() );
        drawCube();
      }
      glPopMatrix();
    }
    
    //dessiner les polys du modele
  
    //dessiner les enfants du noeud
    for( unsigned int i = 0; i < ipObjectNode->getChildCount(); i++ )
    {
      drawScene( ipObjectNode->getChild( i ) );
    }
  }
  glPopMatrix();
}


void
RealEdit3d::drawCube()
{
  glPushAttrib( GL_CURRENT_BIT | GL_POLYGON_BIT | GL_ENABLE_BIT );

  glDisable( GL_LIGHTING );
  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  glColor3f( 0, 85/255.0, 176/255.0);
  glCallList( mCube );
  
  glPopAttrib();
}

void
RealEdit3d::initDisplayList()
{
  if ( mCube != kInvalidDisplayList )
  { return; }
  
  float hs = 0.5;
  
  mCube = glGenLists(1);
  glNewList( mCube, GL_COMPILE );
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
