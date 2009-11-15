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
#include "MathUtils.h"
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
  
  //encode the id into color for color picking
  void idToColor(unsigned int iId)
  {
    int r,g,b,a;
    r = g = b = a = 0;
    int remaining = iId / 255;
    a = iId % 255;
    b = remaining % 255;
    remaining /= 255;
    g = remaining % 255;
    remaining /= 255;
    r = remaining % 255;
    
    glColor4ub(r, g, b, a);
  }
  
  //de-encode color to id.
  int colorToId(const Color& iC)
  { return iC.a + iC.b *255 + iC.g * 255*255 + iC.r *255*255*255; }

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
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, getCamera().getWindowInfo().getWidth(),
    0, getCamera().getWindowInfo().getHeight(),
    -1, 1000);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

    GLdouble modelView[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    // get 3D coordinates based on window coordinates
    double x,y,z;
    gluUnProject(getCamera().getWindowInfo().getWidth() - 25,
      25,
      0.01,
      modelView, projection, viewport,
      &x, &y, &z);
    glTranslated(x, y, z);

    glScaled(20.0, 20.0, 20.0);
    mDisplayData.drawAxis();
  
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  
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
    mDisplayData.drawBoundingBox(iM.getBoundingBox().getMin(), 
      iM.getBoundingBox().getMax());
  glPopAttrib();
}

//------------------------------------------------------------------------------
void RealEdit3d::drawEdges(const RealEditModel& iM) const
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  map<unsigned int, RealEditPolygon>::const_iterator it =
    iM.getPolygons().begin();
  for(; it != iM.getPolygons().end(); ++it)
  {
    const RealEditPolygon& poly = it->second;    
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
  map<unsigned int, RealEditPolygon>::const_iterator it =
    iM.getPolygons().begin();
  for(; it != iM.getPolygons().end(); ++it)
  {
    const RealEditPolygon& poly = it->second;
    
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
  map<unsigned int, RealEditPoint>::const_iterator it =
    iM.getPoints().begin();
    
  glPushAttrib(GL_POINT_BIT | GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glPointSize(6);

  glBegin(GL_POINTS);
  for(; it != iM.getPoints().end(); ++it)
  {
    const RealEditPoint& p = it->second;
    
    if(mEditionData.isSelected(p.getId()))
      color(kcSelectedPoint);
    else if(p.getId() == mHoverId)
      color(kcHover);
    else
      color(kcPoint);
    
    if(iPicking)
      idToColor(p.getId());
    
    glVertex3d(p.pos().getX(), p.pos().getY(), p.pos().getZ());
  }
  glEnd();
  glPopAttrib();
}

//------------------------------------------------------------------------------
void RealEdit3d::drawPolygons(const RealEditModel& iM,
  bool iPicking /*= false*/) const
{
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0, 2.0);
  map<unsigned int, RealEditPolygon>::const_iterator it =
    iM.getPolygons().begin();
    
  for(; it != iM.getPolygons().end(); ++it)
  {
    const RealEditPolygon& poly = it->second;
    
    if(mEditionData.isSelected(poly.getId()))
      color(kcSelectedPolygon);
    else if(poly.getId() == mHoverId)
      color(kcHover);
    else
      color(kcPolygon);
    
    if(iPicking)
      idToColor(poly.getId());
    
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
        drawPolygons(model);
      glPopAttrib();
      
      if (ipObjectNode == mEditionData.getCurrentNode())
      {
        //dessiner les lignes du polygon
        glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT |
                     GL_COLOR_BUFFER_BIT | GL_HINT_BIT | GL_DEPTH_BUFFER_BIT);
          glDisable(GL_LIGHTING);
          enableSmoothLines();
          drawEdges(model);
        glPopAttrib();
      }
    }
    else  //Edition Mode
    {
      //dessiner les polys du modele
      glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT);
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
          enableSmoothLines();
          drawEdges(model);
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
      //On calcul le delta (pixel) de la souris en deltaGL.
      //on prend -Y pcq l'axe pixel est inversé de l'axe GL
      Vector3d deltaGL = getCamera().pixelDeltaToGLDelta(mMouseInfo.delta.x(),
        -mMouseInfo.delta.y());
      getCamera().move(-deltaGL);
      update();
    }
    break;
    case msIdle:
      switch (mController.getTool())
      {
        case Controller::tSelection:
        {
          vector<unsigned int> hits = pick(e->x(), e->y());
          if(!hits.empty())
          { mHoverId = hits.front(); }
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
          
          vector<unsigned int> hits = pick(e->x(), e->y());
          //puisque les Hits sont tries en Z, on prend le premier de la liste
          //en sachant que c'est le plus proche de la camera.
          if(!hits.empty())
            mController.select(hits.front(), sMode);
          update();
        }
        break;
        case Controller::tTranslation:
        {
          //On calcul le delta (pixel) de la souris en deltaGL.
          //on prend -Y pcq l'axe pixel est inversé de l'axe GL
          Point3d c = mEditionData.getCurrentModel().getCentroid();
          Vector3d deltaGL =
            getCamera().pixelDeltaToGLDelta(mMouseInfo.delta.x(),
              -mMouseInfo.delta.y(), c);
          mController.translate(deltaGL);
        }
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
           
          vector<unsigned int> hits = pick(e->x(), e->y());
          //puisque les Hits sont tries en Z, on prend le premier de la liste
          //en sachant que c'est le plus proche de la camera.
          if(!hits.empty())
            mController.select(hits.front(), sMode);
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
        case Controller::tTranslation: mController.translateEnd(); break;
        default: break;
      }
      break;
    default: setMouseState(msIdle); break;
  }
  
  changeCursor();
}

//------------------------------------------------------------------------------
/*voir http://www.lighthouse3d.com/opengl/picking/index.php?color1
  pour plus d'info*/
vector<unsigned int> RealEdit3d::pick(int iX, int iY, int iWidth /*= 1*/,
  int iHeight /*= 1*/ )
{
  vector<unsigned int> hits;
	GLint viewport[4];
	GLubyte pixel[4];

	glGetIntegerv(GL_VIEWPORT,viewport);

  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DITHER);
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  //draw the scene in picking mode...
  Widget3d::paintGL();
  drawSceneForPicking(mEditionData.getScene().getObjectNode());
  
	glReadPixels(iX, viewport[3]- iY, 1, 1,
		GL_RGBA,GL_UNSIGNED_BYTE,(void *)pixel);
  
  hits.push_back(colorToId(Color(pixel[0],pixel[1],pixel[2],pixel[3])));
  glPopAttrib();

	printf("Color: %d %d %d %d\n",pixel[0],pixel[1],pixel[2],pixel[3]);
  
//  float z = 0.0;
//  glReadPixels(iX, viewport[3]- iY, 1, 1,
//		 GL_DEPTH_COMPONENT, GL_FLOAT, &z );
//  printf("depth: %f\n",z);
  
  
  return hits;
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


