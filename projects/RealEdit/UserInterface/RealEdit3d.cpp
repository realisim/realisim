/*
 *  RealEdit3d.cpp
 *  Project
 *
 *  Created by Pierre-Olivier Beaudoin on 17/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DataModel.h"
#include "DisplayData.h"
#include "EditionData.h"
#include "RealEdit3d.h"
#include "ObjectNode.h"

using namespace Realisim;
using namespace RealEdit;

namespace
{
  const int kInvalidDisplayList = -1;
}


RealEdit3d::RealEdit3d( QWidget* ipParent, 
                        const QGLWidget* iSharedWidget, 
                        const DisplayData& iDisplayData,
                        const EditionData& iEditionData ) : 
Widget3d(ipParent, iSharedWidget),
mDisplayData(iDisplayData),
mEditionData( iEditionData )
{
}

RealEdit3d::~RealEdit3d()
{
}

//------------------------------------------------------------------------------
void RealEdit3d::currentNodeChanged()
{
  Camera cam = getCamera();
  Path p(mEditionData.getCurrentNode());
  cam.setTransformation(p.getSceneTransformation());
  setCamera( cam );
}

//------------------------------------------------------------------------------
void RealEdit3d::drawPolygons(const RealEditModel* ipModel) const
{
  glBegin(GL_POLYGON);
  for(unsigned int i = 0; i < ipModel->getPolygonCount(); ++i)
  {
    const RealEditPolygon* pPoly = ipModel->getPolygon(i);
    glNormal3d(pPoly->getNormals()[0].getX(),
               pPoly->getNormals()[0].getY(),
               pPoly->getNormals()[0].getZ());
    glVertex3d(pPoly->getPoints()[0]->getX(),
               pPoly->getPoints()[0]->getY(),
               pPoly->getPoints()[0]->getZ());
    glNormal3d(pPoly->getNormals()[1].getX(),
               pPoly->getNormals()[1].getY(),
               pPoly->getNormals()[1].getZ());
    glVertex3d(pPoly->getPoints()[1]->getX(),
               pPoly->getPoints()[1]->getY(),
               pPoly->getPoints()[1]->getZ());
    glNormal3d(pPoly->getNormals()[2].getX(),
               pPoly->getNormals()[2].getY(),
               pPoly->getNormals()[2].getZ());
    glVertex3d(pPoly->getPoints()[2]->getX(),
               pPoly->getPoints()[2]->getY(),
               pPoly->getPoints()[2]->getZ());
  }
  glEnd();
}

//------------------------------------------------------------------------------
void
RealEdit3d::drawScene(const RealEdit::ObjectNode* ipObjectNode) const
{
  const RealEditModel* pModel = ipObjectNode->getModel();
  
  glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT );
  glPushMatrix();
  {
    //appliquer la transfo du noeud
    glMultMatrixd( ipObjectNode->getTransformation().getPtr() );
    
    //dessiner les points du modele
    for( unsigned int i = 0; i < pModel->getPointCount(); ++i )
    {
      glPushMatrix();
      {
        const RealEditPoint* pPoint = pModel->getPoint(i);
        glTranslated( pPoint->getX(), pPoint->getY(), pPoint->getZ() );
        mDisplayData.drawCube();
      }
      glPopMatrix();
    }
    
    //dessiner les polys du modele
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(1.0, 3.0);
      drawPolygons(pModel);
    glPopAttrib();
    
    //dessiner les lignes du polygon
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
      glDisable(GL_LIGHTING);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glColor3d( 0.0, 1.0, 0.2);
      drawPolygons(pModel);
    glPopAttrib();
    
    //dessiner les enfants du noeud
    for( unsigned int i = 0; i < ipObjectNode->getChildCount(); i++ )
    {
      drawScene( ipObjectNode->getChild( i ) );
    }
  }
  glPopMatrix();
  glPopAttrib();
}

//------------------------------------------------------------------------------
void
RealEdit3d::paintGL()
{
  Widget3d::paintGL();
  drawScene( mEditionData.getScene().getObjectNode() );
}


