/*
 *  RealEdit3d.cpp
 *  Project
 *  Created by Pierre-Olivier Beaudoin on 17/11/08.
 */

#include <algorithm>
#include "DataModel.h"
#include "DisplayData.h"
#include "EditionData.h"
#include "MathDef.h"
#include "ObjectNode.h"
#include <QCursor>
#include <QMouseEvent>
#include "RealEdit3d.h"
#include "Vect.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
using namespace realEdit;
using namespace std;

namespace
{
  const int kDragTreshold = 3; //3 pixels to go into drag mode
}

RealEdit3d::RealEdit3d (QWidget* ipParent, 
                        const QGLWidget* ipSharedWidget, 
                        Controller& iC) : 
Widget3d(ipParent, ipSharedWidget),
mController (iC),
mDisplayData (iC.getDisplayData ()),
mEditionData (const_cast<const Controller&>(iC).getEditionData ()),
mMouseInfo(),
mMouseState(msIdle),
mPreviousTool(mController.getTool())
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
}

RealEdit3d::~RealEdit3d()
{}

//------------------------------------------------------------------------------
/*Donner la transformation du noeud courant à la caméra.*/
void RealEdit3d::changeCurrentNode()
{
  Camera cam = getCamera();
  Path p(mEditionData.getCurrentNode());
  cam.setTransformation(p.getNodeToScene());
  setCamera( cam );
}

//------------------------------------------------------------------------------
void RealEdit3d::changeCursor()
{
  switch (getMouseState()) 
  {
    case msCamera:
      setCursor(Qt::OpenHandCursor);
      break;
    case msCameraDrag:
      setCursor(Qt::ClosedHandCursor);
      break;
    case msIdle:
      switch (mController.getTool()) 
      {
        case Controller::tSelection:
          setCursor(Qt::ArrowCursor);
          break;
        default:
          setCursor(Qt::ArrowCursor);
          break;
      } 
      break;
    case msDown:
      break;
    case msDrag:
      break;
    default:
      setCursor(Qt::ArrowCursor);
      break;
  }
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
void RealEdit3d::drawBoundingBox(const RealEditModel& iM,
  bool iPicking /*= false*/) const
{
  if(iPicking)
  {
    glPushName(iM.getId());
    glPushName(Hits::tModel);
  }

  glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT |
                 GL_COLOR_BUFFER_BIT | GL_HINT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    enableSmoothLines();
    treeD::BoundingBox bb(iM.getBoundingBox().getMin(), 
      iM.getBoundingBox().getMax() );
    bb.draw();
  glPopAttrib();
  
  if(iPicking)
  {
    glPopName();
    glPopName();
  }
}

//------------------------------------------------------------------------------
void RealEdit3d::drawLines(const RealEditModel& iM) const
{
  for(unsigned int i = 0; i < iM.getPolygonCount(); i++)
  {
    const RealEditPolygon& poly = iM.getPolygon(i);
    
    glColor4d( 0.0, 1.0, 0.2, 1.0);
    
    glBegin(GL_POLYGON);
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
void RealEdit3d::drawNormals(const RealEditModel& iM) const
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
void RealEdit3d::drawPoints(const RealEditModel& iM,
  bool iPicking /*= false*/) const
{
  //dessiner les points du modele
  for (unsigned int i = 0; i < iM.getPointCount(); ++i)
  {
    const RealEditPoint& p = iM.getPoint (i);
    
    if(mEditionData.isSelected(p.getId()))
      glColor3f(1.0, 0.0, 0.0);
    else
      glColor3d(0, 85/255.0, 176/255.0);
    
    if(iPicking)
    {
      glPushName(p.getId());
      glPushName(Hits::tPoint);
    }
    
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
    
    if(iPicking)
    {
      glPopName();
      glPopName();
    }
  }
}

//------------------------------------------------------------------------------
void RealEdit3d::drawPolygons(const RealEditModel& iM,
  bool iPicking /*= false*/) const
{
  for(unsigned int i = 0; i < iM.getPolygonCount(); i++)
  {
    const RealEditPolygon& poly = iM.getPolygon(i);
    
    if(mEditionData.isSelected(poly.getId()))
      glColor3f(1.0, 0.0, 0.0);
    else
      glColor3f(0.85, 0.85, 0.85);
    
    if(iPicking)
    {
      glPushName(poly.getId());
      glPushName(Hits::tPolygon);
    }
    
    glBegin(GL_POLYGON);
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
    
    if(iPicking)
    {
      glPopName();
      glPopName();
    }  
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
    
    if(mController.getMode() == Controller::mAssembly)
    {
      //dessine le boundingBox
      drawBoundingBox(model);
      
      //dessiner les polys du modele
      glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 3.0);
        drawPolygons(model);
      glPopAttrib();
      
      if (ipObjectNode == mEditionData.getCurrentNode())
      {
        //dessiner les lignes du polygon
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT |
                     GL_COLOR_BUFFER_BIT | GL_HINT_BIT | GL_DEPTH_BUFFER_BIT);
          glDisable(GL_LIGHTING);
          glDisable(GL_CULL_FACE);
          enableSmoothLines();
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          drawLines(model);
        glPopAttrib();
      }
    }
    else  //Edition Mode
    {
      //dessiner les polys du modele
      glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 3.0);
        drawPolygons(model);
      glPopAttrib();
      
      if (ipObjectNode == mEditionData.getCurrentNode())
      {
                
        //dessiner les lignes du polygon
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT |
                     GL_COLOR_BUFFER_BIT | GL_HINT_BIT | GL_DEPTH_BUFFER_BIT);
          //dessine les points du model
          drawPoints(model);
         
          glDisable(GL_LIGHTING);
          glDisable(GL_CULL_FACE);
          enableSmoothLines();
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          drawLines(model);
        glPopAttrib();
      
        //dessine les normals du modèle
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT |
                     GL_HINT_BIT);
          glDisable(GL_LIGHTING);
          glColor4d(1.0, 1.0, 1.0, 1.0);
          enableSmoothLines();
          drawNormals(model);
        glPopAttrib();
      }
    }
    
    //dessiner les enfants du noeud
    for( unsigned int i = 0; i < ipObjectNode->getChildCount(); i++ )
      drawScene(ipObjectNode->getChild(i));
  }
  glPopMatrix();
}

//------------------------------------------------------------------------------
void
RealEdit3d::drawSceneForPicking(const realEdit::ObjectNode* ipObjectNode) const
{
  const RealEditModel model = ipObjectNode->getModel();
  
  glPushMatrix();
  {
    //appliquer la transfo du noeud
    glMultMatrixd( ipObjectNode->getTransformation().getPtr() );
    
    if(mController.getMode() == Controller::mAssembly)
      drawBoundingBox(model, true);
    else  //Edition Mode
    {
      //dessiner les polys du modele
      glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
        drawPolygons(model);
      glPopAttrib();
      
      if (ipObjectNode == mEditionData.getCurrentNode())
      {                 
        //dessiner les points et poly du modele
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
          drawPoints(model, true);
          drawPolygons(model, true);
        glPopAttrib();
      }
    }
    
    //dessiner les enfants du noeud
    for( unsigned int i = 0; i < ipObjectNode->getChildCount(); i++ )
      drawSceneForPicking(ipObjectNode->getChild(i));
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
void RealEdit3d::keyPressEvent(QKeyEvent* e)
{
  makeCurrent();
  
  switch (e->key()) 
  {
    case Qt::Key_Escape:
        setMouseState(msIdle);
      break;
    default:
      break;
  }
  
  changeCursor();
}

//------------------------------------------------------------------------------
void RealEdit3d::mouseDoubleClickEvent(QMouseEvent* e)
{makeCurrent();}

//------------------------------------------------------------------------------
void RealEdit3d::mouseMoveEvent(QMouseEvent* e)
{
  makeCurrent();
  mMouseInfo.delta = Point3i(e->x(), e->y(), 0) - mMouseInfo.end;
  mMouseInfo.end.setXYZ(e->x(), e->y(), 0);
  
  //On calcul le delta (pixel) de la souris en deltaGL.
  //on prend -Y pcq l'axe pixel est inversé de l'axe GL
  Vector3d deltaGL =
    getCamera().pixelDeltaToGLDelta(mMouseInfo.delta.getX(),
      -mMouseInfo.delta.getY());
  
  switch (getMouseState()) 
  {
    case msCamera:
      if( mMouseInfo.end.dist(mMouseInfo.origin) >= kDragTreshold )
        setMouseState(msCameraDrag);
      break;
    case msCameraDrag:
    {
      getCamera().move(-deltaGL);
      update();
    }
      break;
    case msIdle:
      break;
    case msDown:
      if( mMouseInfo.end.dist(mMouseInfo.origin) >= kDragTreshold )
        setMouseState(msDrag);
      break;
    case msDrag:
      switch (mController.getTool())
      {
        case Controller::tTranslation:
          mController.translate(deltaGL);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
  changeCursor();
}

//------------------------------------------------------------------------------
void RealEdit3d::mousePressEvent(QMouseEvent* e)
{
  makeCurrent();
  mMouseInfo.origin.setXYZ(e->x(), e->y(), 0);
  mMouseInfo.end.setXYZ(e->x(), e->y(), 0);
  
  switch (getMouseState()) 
  {
  case msIdle:
      setMouseState(msDown);
      
      /*Le bouton droit de la souris est réservé pour la caméra. Quand la souris
       est idle et que le bouton droit est pressé, l'état caméra s'active. 
       Lorsque le bouton est relaché, on revient a l'état msIdle de l'outil 
       courant.*/
      if(e->button() == Qt::RightButton)
          setMouseState(msCamera);
      break;
  default:
    break;
  }
  
  changeCursor();
}

//------------------------------------------------------------------------------
void RealEdit3d::mouseReleaseEvent(QMouseEvent* e)
{
  makeCurrent();
  switch (getMouseState()) 
  {
    case msCamera:
    case msCameraDrag:
      setMouseState(msIdle);
      break;
    case msIdle:
      break;
    case msDown:
      setMouseState(msIdle);
      switch (mController.getTool()) 
      {
        case Controller::tSelection:
        {
          vector<uint> selectedIds;
          vector<Hits> hits = pick(e->x(), e->y());
          for(uint i = 0; i < hits.size(); ++i)
            selectedIds.push_back(hits[i].getId());
          mController.select(selectedIds);
        }
          break;
        default:
          break;
      }
      break;
    case msDrag:
      setMouseState(msIdle);
      switch (mController.getTool()) 
      {
        case Controller::tSelection:
          break;
        default:
          break;
      }
      break;
    default:
      setMouseState(msIdle);
      break;
  }
  
  changeCursor();
}

//------------------------------------------------------------------------------
/*voir http://www.lighthouse3d.com/opengl/picking/index.php?openglway2 
  pour plus d'info*/
vector<RealEdit3d::Hits> RealEdit3d::pick(int x, int y)
{
  const int BUFSIZE = 1024;
  unsigned int selectBuf[BUFSIZE];
  int hits = 0;
  int viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);
  glSelectBuffer(BUFSIZE, selectBuf);
  (void) glRenderMode(GL_SELECT);

  glPushAttrib(GL_TRANSFORM_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  //Creat a 1 x 1 pixel picking region near the cursor
  gluPickMatrix( (GLdouble) x, (GLdouble) (viewport[3] - y),
                1, 1, viewport );

  //viewport[2]: width 
  //viewport[3]: height
  mCam.projectionGL(viewport[2], viewport[3]);
  glMatrixMode(GL_MODELVIEW);
  glInitNames();
  
  //draw the scene in picking mode...
  Widget3d::paintGL();
  drawSceneForPicking(mEditionData.getScene().getObjectNode());
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glFlush();
  glPopAttrib();

  hits = glRenderMode(GL_RENDER);

  return processHits(hits, selectBuf);
}

//------------------------------------------------------------------------------
/*voir http://www.lighthouse3d.com/opengl/picking/index.php?openglway3 
  pour plus d'info*/
vector<RealEdit3d::Hits> RealEdit3d::processHits(int iHits,
  unsigned int iBuffer[])
{
  int i;
  unsigned int names, *ptr;
  double maxDepth, minDepth;
  vector<Hits> result;

  ptr = (uint*) iBuffer;
  for (i = 0; i < iHits; i++)
  {	/*  for each hit  */
    names = *ptr;
    ptr++;
    minDepth = (double) *ptr/0x7fffffff;
    ptr++;
    maxDepth = (double) *ptr/0x7fffffff;
    ptr++;
    
    assert(names <= 2 && "Au maximum 2 noms. Le Id de l'object et son type");
//    for (uint j = 0; j < names; j++)
//    {	/*  for each name */
//      ptr++;
//    }

    if(names == 2)
    {
      uint id = *ptr;
      ptr++;
      Hits::type t = (Hits::type)*ptr;
      ptr++;
      result.push_back(Hits(minDepth, maxDepth, id, t));
    }
  }
  
  sort(result.begin(), result.end());
  cout<<"Number of hits: "<<result.size()<<endl;
  for(uint i = 0; i < result.size(); ++i)
  {
    cout<<"\t name:"<<result[i].getId()<<
      "\t type: "<<result[i].getType()<<
      "\t min depth: "<<result[i].getMinDepth()<<
      "\t max depth:"<<result[i].getMaxDepth()<<endl;
  }
  return result;
}

//------------------------------------------------------------------------------
void RealEdit3d::paintGL()
{
  Widget3d::paintGL();
  drawScene( mEditionData.getScene().getObjectNode() );
  drawAxis();
}

//------------------------------------------------------------------------------
void RealEdit3d::wheelEvent(QWheelEvent* e)
{
  Widget3d::wheelEvent(e);
}


