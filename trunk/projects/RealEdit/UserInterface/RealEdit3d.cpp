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
  const unsigned int kInvalidDisplayList = 0;
}

GLuint RealEdit3d::mCube = kInvalidDisplayList;

RealEdit3d::RealEdit3d( QWidget* ipParent, 
                        const QGLWidget* iSharedWidget, 
                        const EditionData& iEditionData ) : 
Widget3d(ipParent, iSharedWidget),
mEditionData( iEditionData )
{
  //initDisplayList();
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

   //float hs = 0.5;

   //glBegin(GL_QUADS);
   //   glNormal3f( 0.0F, 0.0F, 1.0F);
   //   glVertex3f( hs, hs, hs); glVertex3f(-hs, hs, hs);
   //   glVertex3f(-hs,-hs, hs); glVertex3f( hs,-hs, hs);
   //   
   //   glNormal3f( 0.0F, 0.0F,-1.0F);
   //   glVertex3f(-hs,-hs,-hs); glVertex3f(-hs, hs,-hs);
   //   glVertex3f( hs, hs,-hs); glVertex3f( hs,-hs,-hs);
   //   
   //   glNormal3f( 0.0F, 1.0F, 0.0F);
   //   glVertex3f( hs, hs, hs); glVertex3f( hs, hs,-hs);
   //   glVertex3f(-hs, hs,-hs); glVertex3f(-hs, hs, hs);
   //   
   //   glNormal3f( 0.0F,-1.0F, 0.0F);
   //   glVertex3f(-hs,-hs,-hs); glVertex3f( hs,-hs,-hs);
   //   glVertex3f( hs,-hs, hs); glVertex3f(-hs,-hs, hs);
   //   
   //   glNormal3f( 1.0F, 0.0F, 0.0F);
   //   glVertex3f( hs, hs, hs); glVertex3f( hs,-hs, hs);
   //   glVertex3f( hs,-hs,-hs); glVertex3f( hs, hs,-hs);
   //   
   //   glNormal3f(-1.0F, 0.0F, 0.0F);
   //   glVertex3f(-hs,-hs,-hs); glVertex3f(-hs,-hs, hs);
   //   glVertex3f(-hs, hs, hs); glVertex3f(-hs, hs,-hs);
   // glEnd();
  
  glPopAttrib();
}

void
RealEdit3d::initDisplayList()
{
  if ( mCube != kInvalidDisplayList )
  { return; }
  

  //const double Pi = 3.14159265358979323846;

  //mCube = glGenLists(1);
  //glNewList(mCube, GL_COMPILE);
  ////glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectance);

  //GLdouble r0 = 1.0;//innerRadius;
  //GLdouble r1 = 4/*outerRadius*/ - toothSize / 2.0;
  //GLdouble r2 = 4/*outerRadius*/ + toothSize / 2.0;
  //GLdouble delta = (2.0 * Pi / toothCount) / 4.0;
  //GLdouble z = thickness / 2.0;
  //int i, j;

  //glShadeModel(GL_FLAT);

  //for (i = 0; i < 2; ++i) {
  //  GLdouble sign = (i == 0) ? +1.0 : -1.0;

  //  glNormal3d(0.0, 0.0, sign);

  //  glBegin(GL_QUAD_STRIP);
  //  for (j = 0; j <= toothCount; ++j) {
  //    GLdouble angle = 2.0 * Pi * j / toothCount;
  //    glVertex3d(r0 * cos(angle), r0 * sin(angle), sign * z);
  //    glVertex3d(r1 * cos(angle), r1 * sin(angle), sign * z);
  //    glVertex3d(r0 * cos(angle), r0 * sin(angle), sign * z);
  //    glVertex3d(r1 * cos(angle + 3 * delta), r1 * sin(angle + 3 * delta),
  //      sign * z);
  //  }
  //  glEnd();

  //  glBegin(GL_QUADS);
  //  for (j = 0; j < toothCount; ++j) {
  //    GLdouble angle = 2.0 * Pi * j / toothCount;
  //    glVertex3d(r1 * cos(angle), r1 * sin(angle), sign * z);
  //    glVertex3d(r2 * cos(angle + delta), r2 * sin(angle + delta),
  //      sign * z);
  //    glVertex3d(r2 * cos(angle + 2 * delta), r2 * sin(angle + 2 * delta),
  //      sign * z);
  //    glVertex3d(r1 * cos(angle + 3 * delta), r1 * sin(angle + 3 * delta),
  //      sign * z);
  //  }
  //  glEnd();
  //}

  //glBegin(GL_QUAD_STRIP);
  //for (i = 0; i < toothCount; ++i) {
  //  for (j = 0; j < 2; ++j) {
  //    GLdouble angle = 2.0 * Pi * (i + (j / 2.0)) / toothCount;
  //    GLdouble s1 = r1;
  //    GLdouble s2 = r2;
  //    if (j == 1)
  //      qSwap(s1, s2);

  //    glNormal3d(cos(angle), sin(angle), 0.0);
  //    glVertex3d(s1 * cos(angle), s1 * sin(angle), +z);
  //    glVertex3d(s1 * cos(angle), s1 * sin(angle), -z);

  //    glNormal3d(s2 * sin(angle + delta) - s1 * sin(angle),
  //      s1 * cos(angle) - s2 * cos(angle + delta), 0.0);
  //    glVertex3d(s2 * cos(angle + delta), s2 * sin(angle + delta), +z);
  //    glVertex3d(s2 * cos(angle + delta), s2 * sin(angle + delta), -z);
  //  }
  //}
  //glVertex3d(r1, 0.0, +z);
  //glVertex3d(r1, 0.0, -z);
  //glEnd();

  //glShadeModel(GL_SMOOTH);

  //glBegin(GL_QUAD_STRIP);
  //for (i = 0; i <= toothCount; ++i) {
  //  GLdouble angle = i * 2.0 * Pi / toothCount;
  //  glNormal3d(-cos(angle), -sin(angle), 0.0);
  //  glVertex3d(r0 * cos(angle), r0 * sin(angle), +z);
  //  glVertex3d(r0 * cos(angle), r0 * sin(angle), -z);
  //}
  //glEnd();

  //glEndList();


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
