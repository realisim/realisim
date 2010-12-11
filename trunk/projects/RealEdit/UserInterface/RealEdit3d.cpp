/*
 *  RealEdit3d.cpp
 *  Project
 *  Created by Pierre-Olivier Beaudoin on 17/11/08.
 */

#include <algorithm>
#include "Commands/select.h"
#include "DataModel/DataModel.h"
#include "DataModel/DisplayData.h"
#include "DataModel/EditionData.h"
#include "math/MathDef.h"
#include "math/MathUtils.h"
#include "DataModel/ObjectNode.h"
#include <QCursor>
#include <QMouseEvent>
#include <QRect>
#include "UserInterface/RealEdit3d.h"
#include <set>
#include "math/Vect.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
using namespace realEdit;
  using namespace commands;
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
  unsigned int colorToId(const Color& iC)
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
mEditionData (iC.getEditionData ()),
mMouseInfo(),
mMouseState(msIdle),
mPreviousTool(mController.getTool()),
mShowSelectionBox(false),
mHoverId(0),
mpSelect(0)
{
//  setFocusPolicy(Qt::StrongFocus);
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
  cam.setTransformationToGlobal(p.getNodeToScene());
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
        case Controller::tSelect:
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
    idToColor(iM.getId());

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
  
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DITHER);
  glDisable(GL_FOG);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_3D);
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  
  glPushMatrix();
  {
    //appliquer la transfo du noeud
    glMultMatrixd( ipObjectNode->getTransformation().getPtr() );
    
    if(mController.getMode() == Controller::mAssembly)
      drawBoundingBox(model, true);
    else  //Edition Mode
    {
      //dessiner les points et poly du modele courant.
      if( ipObjectNode == mEditionData.getCurrentNode() )
      {
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
  glPopAttrib();
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
//voir mouseMoveEvent
void RealEdit3d::keyPressEvent(QKeyEvent* e)
{
  makeCurrent();
  
  //on elimine tout les repeats
  if(e->isAutoRepeat())
    return;
  
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
//voir mouseMoveEvent
void RealEdit3d::keyReleaseEvent(QKeyEvent* e)
{
  switch (e->key())
  { 
    default: break;
  }
  changeCursor();
}

//------------------------------------------------------------------------------
//voir mouseMoveEvent
void RealEdit3d::mouseDoubleClickEvent(QMouseEvent* e)
{makeCurrent();}

//------------------------------------------------------------------------------
/*On pourrait penser que ces méthodes devraient être dans le controller parce 
  qu'elles contiennent la logique des outils. La gestion de la machine à état
  est responsable de l'expérience de l'usager. C'est pourquoi je considère que 
  cette logique de gestion des outils fait partie du l'interface usager et c'est
  pourquoi ce code est ici et non dans Controller.  */
void RealEdit3d::mouseMoveEvent(QMouseEvent* e)
{
  makeCurrent();
  mMouseInfo.delta = mapFromGlobal( QPoint(e->globalX(), e->globalY()) ) - mMouseInfo.end;
  /*mouse delta vaut (0, 0) quand on appel mouseMoveEvent a partir de celle-ci
  avec le même évènement (voir case msDown:). Ça veut dire qu'on vient de passer
  le drag threshold. Le mouse delta doit donc être de (end - origin)*/
  if(mMouseInfo.delta == QPoint(0, 0))
    mMouseInfo.delta = mMouseInfo.end - mMouseInfo.origin;
  mMouseInfo.end = mapFromGlobal( QPoint(e->globalX(), e->globalY()) );
  
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
      mCam.move(-deltaGL);
      update();
    }
    break;
    case msIdle:
      switch (mController.getTool())
      {
        case Controller::tSelect:
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
        case Controller::tSelect:
        {
          if( bool(e->modifiers() & Qt::ShiftModifier) )
          {
            showSelectionBox(true);
            update();
          }
          else
          {
            Select::mode sMode = Select::mAdditive;
            if(e->modifiers() == Qt::AltModifier)
              sMode = Select::mSubtractive;
              
            vector<unsigned int> hits = pick(e->x(), e->y());
            if(!hits.empty())
              mpSelect->update(hits.front(), sMode);
          }

        }
        break;
        case Controller::tTranslate:
        {
          //On calcul le delta (pixel) de la souris en deltaGL.
          //on prend -Y pcq l'axe pixel est inversé de l'axe GL
          Point3d c = mEditionData.getCurrentModel().getCentroid();
          Vector3d deltaGL =
            getCamera().pixelDeltaToGLDelta(mMouseInfo.delta.x(),
              -mMouseInfo.delta.y(), c);
//          mController.translate(deltaGL);
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
//voir mouseMoveEvent
void RealEdit3d::mousePressEvent(QMouseEvent* e)
{
  makeCurrent();
  mMouseInfo.origin = QPoint(e->x(), e->y());
  mMouseInfo.end = QPoint(e->x(), e->y());
  
  switch (getMouseState()) 
  {
  case msIdle:
    if(e->buttons() == Qt::RightButton)
      setMouseState(msCamera);
		else
    {  
      setMouseState(msDown);
      switch (mController.getTool()) 
      {
        case Controller::tSelect:
          {
            if(!mpSelect)
              mpSelect = new Select(mController);                
             
            /*Shift ne doit pas être enfoncé si on veut sélectionner un item*/
            if(!(bool)(e->modifiers() & Qt::ShiftModifier))
            {
              Select::mode sMode = Select::mNormal;
              if(e->modifiers() == Qt::ControlModifier)
                sMode = Select::mAdditive;
              else if(e->modifiers() == Qt::AltModifier)
                sMode = Select::mSubtractive;
                
              vector<unsigned int> hits = pick(e->x(), e->y());          
              if(!hits.empty())
                mpSelect->update(hits.front(), sMode);
              else
                mpSelect->update(0, sMode);
            }
          }
          break;
        default: break;
      }
      break;
    }
  default: break;
  }
  
  changeCursor();
}

//------------------------------------------------------------------------------
//voir mouseMoveEvent
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
        case Controller::tSelect: 
          mpSelect->execute();
          mController.addCommand(mpSelect);
          mpSelect = 0;
          break;
        default: break;
      }      
      break;
    case msDrag:
      setMouseState(msIdle);
      switch (mController.getTool()) 
      {
        case Controller::tSelect:
          if(isSelectionBoxShown())
					{
            showSelectionBox(false);
            Select::mode sMode = Select::mNormal;
          	if( bool(e->modifiers() & Qt::ControlModifier) == true)
            	sMode = Select::mAdditive;
            else if( (bool)(e->modifiers() & Qt::AltModifier) == true)
              sMode = Select::mSubtractive;
            else
              sMode = Select::mNormal;              

						QPoint selectionBoxSize = mMouseInfo.end - mMouseInfo.origin;
            vector<unsigned int> hits = pick(mMouseInfo.origin.x(),
              mMouseInfo.origin.y(), selectionBoxSize.x(), selectionBoxSize.y());
            if(!hits.empty())
              mpSelect->update(hits, sMode);
          }

          
          mpSelect->execute();
          mController.addCommand(mpSelect);
          mpSelect = 0;
          break;
        case Controller::tTranslate:
          //mController.translateEnd();
          break;
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
  GLint viewport[4]; //x, y, width, height
  glGetIntegerv(GL_VIEWPORT,viewport);
  
  /*glReadpixel prend le coin inférieur gauche et la taille, donc
    on va s'assurer de lui passer le coin inférieur gauche de la boite.*/
  int x1,y1,x2,y2;
  //on definit les 4 coins de la fenetre de selection
  //x1 = iX; y1 = iY; x2 = x1 + iWidth; y2 = y1 + iHeight;
  //on trouve le coin inférieur gauche de la boite de selection
  x1 = min(iX, iX + iWidth);
  y1 = max(iY, iY + iHeight);
  //le coin superieur droit
  x2 = max(iX, iX + iWidth);
  y2 = min(iY, iY + iHeight);
//  cout<<endl;
//  cout<<"vSize: "<<viewport[0]<<" "<<viewport[1]<<" "<<viewport[0]+viewport[2]<<" "<<viewport[1]+viewport[3]<<endl;
//  cout<<"box size: "<<abs(iWidth)<<" "<<abs(iHeight)<<endl;
//  cout<<"ll: "<<x1<<" "<<y1<<endl;
  //on cap le coin inferieur gauche sur le viewport
  x1 = max(x1, viewport[0]);
  y1 = max(y1, viewport[1]);
  x1 = min(x1, viewport[0] + viewport[2]);
  y1 = min(y1, viewport[1] + viewport[3]);
//  cout<<"cap ll: "<<x1<<" "<<y1<<endl;
//  cout<<"ur: "<<x2<<" "<<y2<<endl; 
  //on cap le coin superieur droit sur la taille du viewport.
  x2 = max(x2, viewport[0]);
  y2 = max(y2, viewport[1]);
	x2 = min(x2, viewport[0] + viewport[2]);
  y2 = min(y2, viewport[1] + viewport[3]);
//  cout<<"cap ur: "<<x2<<" "<<y2<<endl;
  
	int absWidth = x2 - x1;
  int absHeight = y1 - y2;
//  cout<<"cap box size: "<<absWidth<<" "<<absHeight<<endl;
  vector<unsigned int> hits;
	GLubyte pixels[absWidth * absHeight * 4];

  glPushAttrib(GL_COLOR_BUFFER_BIT);
  //draw the scene in picking mode...
  /*On s'assure que le clear color est completement noir parce qu'il
    représentera le id 0 et n'interferera donc pas dans la sélection.*/
  glClearColor(0, 0, 0, 0);
  Widget3d::paintGL();
  drawSceneForPicking(mEditionData.getRootNode());
  glPopAttrib();
      
	glReadPixels(x1, viewport[3] - y1, absWidth, absHeight,
		GL_RGBA,GL_UNSIGNED_BYTE,(void *)pixels);
  
  for(int i = 0; i < absHeight; ++i)
    for(int j = 0; j < absWidth; ++j)    
      if(pixels[i*absWidth*4 + j*4] != 0 || pixels[i*absWidth*4 + j*4 + 1] != 0 || pixels[i*absWidth*4 + j*4 + 2] != 0 || pixels[i*absWidth*4 + j*4 + 3] != 0)
        hits.push_back(colorToId(Color(pixels[i*absWidth*4 + j*4],pixels[i*absWidth*4 + j*4 + 1],pixels[i*absWidth*4 + j*4 + 2],pixels[i*absWidth*4 + j*4 + 3])));

  //on s'assure que les hits sont unique
  sort(hits.begin(), hits.end());
  hits.erase(unique(hits.begin(), hits.end()), hits.end());
  
	//printf("Color: %d %d %d %d\n",pixel[0],pixel[1],pixel[2],pixel[3]);
  
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
  drawScene( mEditionData.getRootNode() );
  drawAxis();
  
  //Draw 2d selection box on top of the other
  if(isSelectionBoxShown())
    drawSelectionBox();
    
  showFps();
}

//------------------------------------------------------------------------------
void RealEdit3d::wheelEvent(QWheelEvent* e)
{
  Widget3d::wheelEvent(e);
}


