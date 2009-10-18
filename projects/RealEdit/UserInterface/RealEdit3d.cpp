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
#include <QRect>
#include "RealEdit3d.h"
#include <set>
#include "Vect.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
using namespace realEdit;
using namespace std;

namespace
{
  struct Color
  {
    Color() : r(255), g(255), b(255), a(255) {;}
    Color(GLubyte iR, GLubyte iG, GLubyte iB, GLubyte iA) : 
      r(iR), g(iG), b(iB), a(iA) {;}
    GLubyte r; GLubyte g; GLubyte b; GLubyte a;
  };
  
  void color(const Color& iC) {glColor4ub(iC.r, iC.g, iC.b,iC.a);}

  const int kDragTreshold = 3; //3 pixels to go into drag mode
  const Color kcHover(255, 0, 255, 255);
  const Color kcLine(0, 255, 51, 255);
  const Color kcModel();
  const Color kcNormal(255, 255, 255, 255);
  const Color kcPoint(0, 85, 176, 255);
  const Color kcPolygon(217, 217, 217, 255);
  const Color kcSelectedModel();
  const Color kcSelectedPoint(255, 0, 0, 255);
  const Color kcSelectedPolygon(255, 0, 0, 255);
  const Color kcSelectionBox(255, 255, 255, 255);
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
mPreviousTool(mController.getTool()),
mShowSelectionBox(false),
mHoverId(0)
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
  glPushMatrix();
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
  glPopMatrix();
}

//------------------------------------------------------------------------------
void RealEdit3d::drawBoundingBox(const RealEditModel& iM,
  bool iPicking /*= false*/) const
{
  if(iPicking)
    glLoadName(iM.getId());

  glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT |
                 GL_COLOR_BUFFER_BIT | GL_HINT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    enableSmoothLines();
    treeD::BoundingBox bb(iM.getBoundingBox().getMin(), 
      iM.getBoundingBox().getMax() );
    bb.draw();
  glPopAttrib();
}

//------------------------------------------------------------------------------
void RealEdit3d::drawLines(const RealEditModel& iM) const
{
  for(unsigned int i = 0; i < iM.getPolygonCount(); i++)
  {
    const RealEditPolygon& poly = iM.getPolygon(i);
    
    color(kcLine);
    
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
      color(kcSelectedPoint);
    else
      color(kcPoint);
    
    if(iPicking)
      glLoadName(p.getId());
    
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
}

//------------------------------------------------------------------------------
void RealEdit3d::drawPolygons(const RealEditModel& iM,
  bool iPicking /*= false*/) const
{
  for(unsigned int i = 0; i < iM.getPolygonCount(); i++)
  {
    const RealEditPolygon& poly = iM.getPolygon(i);
    
    if(mEditionData.isSelected(poly.getId()))
      color(kcSelectedPolygon);
    else if(poly.getId() == mHoverId)
      color(kcHover);
    else
      color(kcPolygon);
    
    if(iPicking)
      glLoadName(poly.getId());
    
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
//          drawPoints(model);
         
          glDisable(GL_LIGHTING);
          enableSmoothLines();
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
          drawLines(model);
        glPopAttrib();
      
        //dessine les normals du modèle
//        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT |
//                     GL_HINT_BIT);
//          glDisable(GL_LIGHTING);
//          color(kcNormal);
//          enableSmoothLines();
//          drawNormals(model);
//        glPopAttrib();
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
      bool isCurrentNode = ipObjectNode == mEditionData.getCurrentNode();
      //dessiner les points et poly du modele
      glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
        drawPoints(model, isCurrentNode);
        drawPolygons(model, isCurrentNode);
      glPopAttrib();
    }
    
    //dessiner les enfants du noeud
    for( unsigned int i = 0; i < ipObjectNode->getChildCount(); i++ )
      drawSceneForPicking(ipObjectNode->getChild(i));
  }
  glPopMatrix();
}

//------------------------------------------------------------------------------
void RealEdit3d::drawSelectionBox() const
{
  int windowWidth = getCamera().getWindowInfo().getWidth();
  int windowHeight = getCamera().getWindowInfo().getHeight();
  
  glPushMatrix();
  gluLookAt(0, 0, 5,
    0, 0, 0,
    0, 1, 0);
  
  //on fait un projection 2d de la taille du widget pour pouvoir positionner
  //en coordonné pixel.
  glMatrixMode (GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity ();
  gluOrtho2D (0, windowWidth, 0, windowHeight);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  
  color(kcSelectionBox);
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);//
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, 0x0F0F);
  glBegin(GL_LINE_LOOP);
  
  glVertex2i(mMouseInfo.origin.x(), windowHeight - mMouseInfo.origin.y());
  glVertex2i(mMouseInfo.end.x(), windowHeight - mMouseInfo.origin.y());
  glVertex2i(mMouseInfo.end.x(), windowHeight - mMouseInfo.end.y());
  glVertex2i(mMouseInfo.origin.x(), windowHeight - mMouseInfo.end.y());
  glEnd();
  glPopAttrib();
  
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();

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
        showSelectionBox(false);
        update();
      break;
    default: break;
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
  mMouseInfo.delta = QPoint(e->x(), e->y()) - mMouseInfo.end;
  /*mouse delta vaut (0, 0) quand on appel mouseMoveEvent a partir de celle-ci
  avec le même évènement (voir case msDown:). Ça veut dire qu'on vient de passer
  le drag threshold. Le mouse delta doit donc être de (end - origin)*/
  if(mMouseInfo.delta == QPoint(0, 0))
    mMouseInfo.delta = mMouseInfo.end - mMouseInfo.origin;
  mMouseInfo.end = QPoint(e->x(), e->y());
  
  //On calcul le delta (pixel) de la souris en deltaGL.
  //on prend -Y pcq l'axe pixel est inversé de l'axe GL
  Vector3d deltaGL =
    getCamera().pixelDeltaToGLDelta(mMouseInfo.delta.x(),
      -mMouseInfo.delta.y());
  
  switch (getMouseState()) 
  {
    case msCamera:
    {
      QPoint p = mMouseInfo.end - mMouseInfo.origin;
      if( p.manhattanLength() >= kDragTreshold )
      {
        setMouseState(msCameraDrag);
        mouseMoveEvent(e);
      }
    }
    break;
    case msCameraDrag:
    {
      getCamera().move(-deltaGL);
      update();
    }
    break;
    case msIdle:
      switch (mController.getTool())
      {
        case Controller::tSelection:
        {
          vector<Hits> hits = pick(e->x(), e->y());
          if(!hits.empty())
          { mHoverId = hits.front().getId(); }
          else
            mHoverId = 0;
          update();
        }
        break;
        default: break;
      }
      break;
    case msDown:
      {
        QPoint p = mMouseInfo.end - mMouseInfo.origin;
        if( p.manhattanLength() >= kDragTreshold )
        {
          setMouseState(msDrag);
          mouseMoveEvent(e);
        }
      }
      break;
    case msDrag:
      switch (mController.getTool())
      {
        case Controller::tSelection:
        {
          using namespace commands;
          Selection::mode sMode = Selection::mAdditive;
          
          if(e->modifiers() == Qt::ControlModifier)
            sMode = Selection::mSubtractive;
          
          vector<Hits> hits = pick(e->x(), e->y());
          //puisque les Hits sont tries en Z, on prend le premier de la liste
          //en sachant que c'est le plus proche de la camera.
          if(!hits.empty())
            mController.select(hits.front().getId(), sMode);
          update();
        }
        break;
        case Controller::tTranslation:
          mController.translate(deltaGL);
          break;
        default: break;
      }
      break;
    default: break;
  }
  
  changeCursor();
}

//------------------------------------------------------------------------------
void RealEdit3d::mousePressEvent(QMouseEvent* e)
{
  makeCurrent();
  mMouseInfo.origin = QPoint(e->x(), e->y());
  mMouseInfo.end = QPoint(e->x(), e->y());
  
  switch (getMouseState()) 
  {
  case msIdle:
    /*Le bouton droit de la souris est réservé pour la caméra. Quand la souris
     est idle et que le bouton droit est pressé, l'état caméra s'active. 
     Lorsque le bouton est relaché, on revient a l'état msIdle de l'outil 
     courant.*/
    if(e->button() == Qt::RightButton)
      setMouseState(msCamera);
    else
    {
      setMouseState(msDown);
      switch (mController.getTool()) 
      {
        case Controller::tSelection:
        {
          using namespace commands;
          Selection::mode sMode = Selection::mNormal;
          if(e->modifiers() == Qt::ShiftModifier)
            sMode = Selection::mAdditive;
          else if(e->modifiers() == Qt::ControlModifier)
            sMode = Selection::mSubtractive;
           
          vector<Hits> hits = pick(e->x(), e->y());
          //puisque les Hits sont tries en Z, on prend le premier de la liste
          //en sachant que c'est le plus proche de la camera.
          if(!hits.empty())
            mController.select(hits.front().getId(), sMode);
          else
            mController.select(0, sMode);
        }
          break;
        default: break;
      }
    }
    break;
  default: break;
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
    case msCameraDrag: setMouseState(msIdle); break;
    case msIdle: break;
    case msDown:
      setMouseState(msIdle);
      switch (mController.getTool()) 
      {
        case Controller::tSelection: mController.selectEnd(); break;
        default: break;
      }      
      break;
    case msDrag:
      setMouseState(msIdle);
      switch (mController.getTool()) 
      {
        case Controller::tSelection: mController.selectEnd(); break;
        default: break;
      }
      break;
    default: setMouseState(msIdle); break;
  }
  
  changeCursor();
}

//------------------------------------------------------------------------------
/*voir http://www.lighthouse3d.com/opengl/picking/index.php?openglway2 
  pour plus d'info*/
vector<RealEdit3d::Hits> RealEdit3d::pick(int iX, int iY, int iWidth /*= 1*/,
  int iHeight /*= 1*/ )
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
  gluPickMatrix( (GLdouble) iX, (GLdouble) (viewport[3] - iY),
                iWidth, iHeight, viewport );

  //viewport[2]: width 
  //viewport[3]: height
  getCamera().projectionGL(viewport[2], viewport[3]);
  glMatrixMode(GL_MODELVIEW);
  glInitNames();
  //on ajoute un nom pour initialiser la liste et pouvoir utiliser glLoadName()
  glPushName(0);
  
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
    
    assert(names <= 1 && "Au maximum 2 noms. Le Id de l'object et son type");
//    for (uint j = 0; j < names; j++)
//    {	/*  for each name */
//      ptr++;
//    }

    if(names == 1)
    {
      uint id = *ptr;
      //ptr++;
      //Hits::type t = (Hits::type)*ptr;
      Hits::type t;
      ptr++;
      result.push_back(Hits(minDepth, maxDepth, id, t));
    }
  }
  
  //on enleve le premier element du vector parce que c'est le 0 qu'on a inséré
  result.erase(result.begin());
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
  
  //Draw 2d selection box on top of the other
  if(isSelectionBoxShown())
    drawSelectionBox();
}

//------------------------------------------------------------------------------
void RealEdit3d::wheelEvent(QWheelEvent* e)
{
  Widget3d::wheelEvent(e);
}


