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
  
  if( !pModel )
  {
    return;  //early out
  }
  
  glPushMatrix();
  {
    //appliquer la transfo du noeud
  
    //dessiner les points du modele
    for( unsigned int i = 0; i < pModel->getNumPoint(); ++i )
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
    for( unsigned int i = 0; i < ipObjectNode->getNumChild(); i++ )
    {
      drawScene( ipObjectNode->getChild( i ) );
    }
  }
  glPopMatrix();
}


void
RealEdit3d::drawCube()
{
  float hs = 0.5;
  
  //mDisplayList = glGenLists(1);
  //glNewList(mDisplayList, GL_COMPILE );
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
  //glEndList();
}
