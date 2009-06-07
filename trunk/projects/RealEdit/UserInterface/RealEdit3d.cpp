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
#include "RealEditController.h"
#include "ObjectNode.h"

using namespace realisim;
using namespace realisim::treeD;
using namespace realEdit;
using namespace std;

namespace
{
  const int kInvalidDisplayList = -1;
}

RealEdit3d::RealEdit3d (QWidget* ipParent, 
                        const QGLWidget* iSharedWidget, 
                        RealEditController& iC) : 
Widget3d(ipParent, iSharedWidget),
mController (iC),
mDisplayData (iC.getDisplayData ()),
mEditionData (const_cast<const RealEditController&> (iC).getEditionData ())
{}

RealEdit3d::~RealEdit3d()
{}

//------------------------------------------------------------------------------
void RealEdit3d::currentNodeChanged()
{
  Camera cam = getCamera();
  Path p(mEditionData.getCurrentNode());
  cam.setTransformation(p.getSceneTransformation());
  setCamera( cam );
}

//------------------------------------------------------------------------------
void RealEdit3d::drawAxis() const
{
  mDisplayData.getAxis().setDisplayFlag(Primitives::pViewport);
  mDisplayData.getAxis().setDisplayFlag(Primitives::zViewport);
  //on place la primitive dans le coin inférieur droit
  Point3d p(getCamera().getWindowInfo().getWidth() - 25,
    getCamera().getWindowInfo().getHeight() - 25,
    0);
  mDisplayData.getAxis().setPosition(p);
  Widget3d::applyDisplayFlag(mDisplayData.getAxis());
  //on donne un taille de 20 pixels a la primitives
  glScaled(20.0, 20.0, 20.0);
  mDisplayData.drawAxis();
}

//------------------------------------------------------------------------------
void RealEdit3d::drawNormals(const RealEditModel iM) const
{
  for(unsigned int i = 0; i < iM.getPolygonCount(); i++)
  {
    const RealEditPolygon& poly = iM.getPolygon(i);
    
    glPushMatrix();
    glTranslated(poly.getPoints()[0].x(),
                 poly.getPoints()[0].y(),
                 poly.getPoints()[0].z());
    glBegin(GL_LINES);      
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(poly.getNormals()[0].getX(),
                 poly.getNormals()[0].getY(),
                 poly.getNormals()[0].getZ());
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(poly.getPoints()[1].x (),
                 poly.getPoints()[1].y (),
                 poly.getPoints()[1].z ());
    glBegin(GL_LINES);      
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(poly.getNormals()[1].getX(),
                 poly.getNormals()[1].getY(),
                 poly.getNormals()[1].getZ());
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslated(poly.getPoints()[2].x (),
                 poly.getPoints()[2].y (),
                 poly.getPoints()[2].z ());
    glBegin(GL_LINES);      
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(poly.getNormals()[2].getX(),
                 poly.getNormals()[2].getY(),
                 poly.getNormals()[2].getZ());
    glEnd();
    glPopMatrix();
  }
}

//------------------------------------------------------------------------------
void RealEdit3d::drawPolygons(const RealEditModel iM) const
{
  for(unsigned int i = 0; i < iM.getPolygonCount(); i++)
  {
    glBegin(GL_POLYGON);
    const RealEditPolygon& poly = iM.getPolygon(i);
    glNormal3d(poly.getNormals()[0].getX(),
               poly.getNormals()[0].getY(),
               poly.getNormals()[0].getZ());
    glVertex3d(poly.getPoints()[0].x (),
               poly.getPoints()[0].y (),
               poly.getPoints()[0].z ());    
    glNormal3d(poly.getNormals()[1].getX(),
               poly.getNormals()[1].getY(),
               poly.getNormals()[1].getZ());
    glVertex3d(poly.getPoints()[1].x (),
               poly.getPoints()[1].y (),
               poly.getPoints()[1].z ());
    glNormal3d(poly.getNormals()[2].getX(),
               poly.getNormals()[2].getY(),
               poly.getNormals()[2].getZ());
    glVertex3d(poly.getPoints()[2].x (),
               poly.getPoints()[2].y (),
               poly.getPoints()[2].z ());
    glEnd();
  }
}

//------------------------------------------------------------------------------
void
RealEdit3d::drawScene(const realEdit::ObjectNode* ipObjectNode) const
{
  const RealEditModel model = ipObjectNode->getModel();
  
  glPushMatrix();
  {
    //appliquer la transfo du noeud
    glMultMatrixd( ipObjectNode->getTransformation().getPtr() );
    
    //dessine le boundingBox
    model. getBoundingBox (). draw();
    
    //dessiner les polys du modele
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(1.0, 3.0);
      drawPolygons(model);
    glPopAttrib();
    
    if (ipObjectNode == mEditionData.getCurrentNode())
    {
      //dessiner les points du modele
      for (unsigned int i = 0; i < model.getPointCount(); ++i)
      {
        const RealEditPoint& p = model.getPoint (i);
        glPushMatrix();
        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_HINT_BIT);
          enableSmoothLines();
          mDisplayData.getCube().setPosition (p.pos());        
          //on désactive le zoom de scene sur la primitives cube
          mDisplayData.getCube().setDisplayFlag(Primitives::zViewport);
          Widget3d::applyDisplayFlag(mDisplayData.getCube());
          //on lui donne un scaling de 10 pour que le cube ait toujours
          //5 pixels a l'écran.
          glScaled(5.0, 5.0, 5.0);
          mDisplayData.drawCube();
        glPopAttrib();
        glPopMatrix();
      }
      
      //dessiner les lignes du polygon
      glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT |
                   GL_COLOR_BUFFER_BIT | GL_HINT_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDepthMask(0);
        enableSmoothLines();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor4d( 0.0, 1.0, 0.2, 1.0);
        drawPolygons(model);
      glPopAttrib();
    
//      //dessine les normals du modèle
//      glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT |
//                   GL_HINT_BIT);
//        glDisable(GL_LIGHTING);
//        glColor4d(1.0, 1.0, 1.0, 1.0);
//        enableSmoothLines();
//        drawNormals(model);
//      glPopAttrib();
    }
    
    //dessiner les enfants du noeud
    for( unsigned int i = 0; i < ipObjectNode->getChildCount(); i++ )
    {
      drawScene( ipObjectNode->getChild( i ) );
    }
  }
  glPopMatrix();
}

//------------------------------------------------------------------------------
//active les etats gl pour l'antialisaing sur les lignes
//ATTENTION!!! mettre le 
//pushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_HINT_BIT) avant d'appeler
//cette fonction
void RealEdit3d::enableSmoothLines() const
{
  glEnable (GL_LINE_SMOOTH);
  glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//------------------------------------------------------------------------------
void
RealEdit3d::paintGL()
{
  Widget3d::paintGL();
  drawScene( mEditionData.getScene().getObjectNode() );
  drawAxis();
}


