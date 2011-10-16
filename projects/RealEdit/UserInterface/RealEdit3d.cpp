/*
 *  RealEdit3d.cpp
 *  Project
 *  Created by Pierre-Olivier Beaudoin on 17/11/08.
 */

#include <algorithm>
#include "Commands/select.h"
#include "Commands/translate.h"
#include "DataModel/DataModel.h"
#include "DataModel/DisplayData.h"
#include "DataModel/EditionData.h"
#include "DataModel/ObjectNode.h"
#include "math.h"
#include "math/MathDef.h"
#include "math/MathUtils.h"
#include <QCursor>
#include <QMouseEvent>
#include <QRect>
#include "UserInterface/RealEdit3d.h"
#include <set>
#include "math/Vect.h"
#include "3d/Utilities.h"

using namespace realisim;
  using namespace math;
  using namespace treeD;
  	using namespace utilities;
using namespace realEdit;
  using namespace commands;
using namespace std;

namespace
{
  void color(const QColor& iC) {glColor4ub(iC.red(), iC.green(), iC.blue(), iC.alpha());}

  const int kDragTreshold = 3; //3 pixels to go into drag mode
  const QColor kcHover(255, 0, 255, 255);
  const QColor kcModel();
  const QColor kcNormal(255, 255, 255, 255);
  const QColor kcPoint(0, 85, 176, 255);
  const QColor kcPolygon(217, 217, 217, 255);
  const QColor kcSegment(0, 255, 51, 255);
  const QColor kcSelectedModel();
  const QColor kcSelectedPoint(255, 0, 0, 255);
  const QColor kcSelectedPolygon(255, 0, 0, 255);
  const QColor kcSelectedSegment(255, 0, 0, 255);
  const QColor kcSelectionBox(255, 255, 255, 255);
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
mpSelect(0),
mpTranslate(0)
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
    color(idToColor(iM.getId()));

  glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT |
                 GL_COLOR_BUFFER_BIT | GL_HINT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    //enableSmoothLines();
    mDisplayData.drawBoundingBox(iM.getBoundingBox().getMin(), 
      iM.getBoundingBox().getMax());
  glPopAttrib();
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
      glVertex3dv(poly.getNormals()[0].getPtr());
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(poly.getPoints()[1].x (),
                 poly.getPoints()[1].y (),
                 poly.getPoints()[1].z ());
    glBegin(GL_LINES);      
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3dv(poly.getNormals()[1].getPtr());
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslated(poly.getPoints()[2].x (),
                 poly.getPoints()[2].y (),
                 poly.getPoints()[2].z ());
    glBegin(GL_LINES);      
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3dv(poly.getNormals()[2].getPtr());
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
      color(idToColor(p.getId()));
    
    glVertex3dv(p.pos().getPtr());
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
      color(idToColor(poly.getId()));
    
    glBegin(GL_POLYGON);
    glNormal3dv(poly.getNormals()[0].getPtr());
    glVertex3dv(poly.getPoints()[0].pos().getPtr());    
    glNormal3dv(poly.getNormals()[1].getPtr());
    glVertex3dv(poly.getPoints()[1].pos().getPtr());
    glNormal3dv(poly.getNormals()[2].getPtr());
    glVertex3dv(poly.getPoints()[2].pos().getPtr());
    glEnd();
  }
}

//------------------------------------------------------------------------------
void RealEdit3d::drawSegments(const RealEditModel& iM,
  bool iPicking /*= false*/) const
{
  map<unsigned int, RealEditSegment>::const_iterator it =
    iM.getSegments().begin();
  for(; it != iM.getSegments().end(); ++it)
  {
    const RealEditSegment& s = it->second;
    if(mEditionData.isSelected(s.getId()))
      color(kcSelectedSegment);
    else if(s.getId() == mHoverId)
      color(kcHover);
    else
      color(kcSegment);
    
    glLineWidth(1.0);
    if(iPicking)
    {
      glLineWidth(5.0);
      color(idToColor(s.getId()));
    }
    
    glBegin(GL_LINES);
    glVertex3dv(s.getPoint1().pos().getPtr());
    glVertex3dv(s.getPoint2().pos().getPtr());
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
          //enableSmoothLines();
          drawSegments(model);
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
          //enableSmoothLines();
          drawSegments(model);
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
/*Surcharge de la classe du base. Cette méthode est appelé lorsqu'on appelle
  la méthode Widget3d::pick(...)*/
void RealEdit3d::drawSceneForPicking() const
{ drawSceneForPicking(mEditionData.getRootNode()); }

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
          drawSegments(model, true);
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
//  glEnable (GL_LINE_SMOOTH);
//  glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
//  glEnable (GL_BLEND);
//  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//------------------------------------------------------------------------------
void RealEdit3d::initializeGL()
{
	Widget3d::initializeGL();
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
    default: e->ignore(); Widget3d::keyPressEvent(e); break;
  }
  
  changeCursor();
}

//------------------------------------------------------------------------------
//voir mouseMoveEvent
void RealEdit3d::keyReleaseEvent(QKeyEvent* e)
{
  switch (e->key())
  { 
    default: e->ignore(); break;
  }
  changeCursor();
}

//------------------------------------------------------------------------------
//voir mouseMoveEvent
void RealEdit3d::mouseDoubleClickEvent(QMouseEvent* ipE)
{
  makeCurrent();
  
 	//bouton droit pour deplacer la caméra a l'endroit visé
  switch (ipE->button()) 
  {
    case Qt::RightButton:
    case Qt::LeftButton:
    {
      const EditionData& e = mController.getEditionData();
      const RealEditModel& m = e.getCurrentModel();
      vector<uint> p = pick(ipE->x(), ipE->y());
      if(!p.empty())
      {
      	uint id = p.front();
				Vector3d normal;
        Camera c = getCamera();
        Point3d aimed;
        if(m.hasPoint(id))
        {        	
        	aimed = m.getPoint(id).pos();
          uint count = 0;
          vector<RealEditPolygon> p = m.getPolygonsContainingPoint(id);
          for(uint i = 0; i < p.size(); ++i )
          {
          	for(uint j = 0; j < p[i].getNormals().size(); ++j )
            {
            	normal += p[i].getNormal(j);
              count++;
            }
          }
          normal /= count;
        }
        else if(m.hasSegment(id))
        {
          aimed = ( m.getSegment(id).getPoint1().pos() +
            m.getSegment(id).getPoint2().pos() ) / 2.0;
          uint count = 0;
          //normal moyenne au point 1
          vector<RealEditPolygon> p = m.getPolygonsContainingPoint( m.getSegment(id).getPoint1().getId());
          for(uint i = 0; i < p.size(); ++i )
          {
          	for(uint j = 0; j < p[i].getNormals().size(); ++j )
            {
            	normal += p[i].getNormal(j);
              count++;
            }
          }
          //normal moyenne au point 2
          p = m.getPolygonsContainingPoint( m.getSegment(id).getPoint2().getId());
          for(uint i = 0; i < p.size(); ++i )
          {
          	for(uint j = 0; j < p[i].getNormals().size(); ++j )
            {
            	normal += p[i].getNormal(j);
              count++;
            }
          }
          normal /= count;
        }
        else if(m.hasPolygon(id))
        {        	
          for(uint i = 0; i < m.getPolygon(id).getPoints().size(); ++i)
          	aimed += m.getPolygon(id).getPoint(i).pos();
          aimed /= m.getPolygon(id).getPoints().size();
          
          normal = m.getPolygon(id).getNormal(0);
        }
        
        if(c.getMode() == Camera::PERSPECTIVE)
        {
        	/*on trouve la position finale que la caméra doit
            avoir pour faire face a la selection*/
          normal.normalise();
          Vector3d lookVec(c.getLook(), c.getPos());
          Point3d newLook = aimed;
					Point3d newPos = newLook + lookVec.norm() * normal;
    
          /*Afin d'èliminer le tanguage, on projete se vecteur
            dans le plan x-z.*/      
          //Projection sur le plan z = 0
          //x et z de la transfo
          Vector3d x = Vector3d(1.0, 0.0, 0.0);
          Vector3d z = Vector3d(0.0, 0.0, 1.0);
				      
    			Vector3d newLat = Vector3d(newPos, newLook) ^ c.getUp();
          Vector3d projLatOnX = x * ( x & newLat );
          Vector3d projLatOnZ = z * ( z & newLat );
          Vector3d projXZ = projLatOnX + projLatOnZ;
          
          Vector3d newUp = projXZ ^ -normal;
          projXZ.normalise();
          newUp.normalise();
          normal.normalise();
          
          c.set(newPos, aimed, newUp);
        }
        else
        {
          Point3d delta = aimed - c.getLook();
          c.set(c.getPos() + delta, aimed, c.getUp());
        }
        
        setCamera(c, true, 330);
      }
    }
      break;
    default: break;
  }
}

//------------------------------------------------------------------------------
/*On pourrait penser que ces méthodes devraient être dans le controller parce 
  qu'elles contiennent la logique des outils. La gestion de la machine à état
  est responsable de l'expérience de l'usager. C'est pourquoi je considère que 
  cette logique de gestion des outils fait partie du l'interface usager et c'est
  pourquoi ce code est ici et non dans Controller.  */
void RealEdit3d::mouseMoveEvent(QMouseEvent* e)
{
  makeCurrent();
  mMouseInfo.delta = QPoint(e->x(), e->y()) - mMouseInfo.end;
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
          mpTranslate->update(deltaGL);
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
  mController.setCanChangeTool(false);
  
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
            assert(!mpSelect && "Il ne devrait pas y avoir de commande au moment \
              ou l'on appuit sur la souris sinon on a une fuite.");
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
        case Controller::tTranslate:
          {
          	assert(!mpTranslate && "Il ne devrait pas y avoir de commande au moment \
              ou l'on appuit sur la souris sinon on a une fuite.");
            if(!mpTranslate)
              mpTranslate = new Translate(mController);
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
    case msIdle:
      switch (mController.getTool()) 
      {
      	case Controller::tSelect:
          delete mpSelect;
		      mpSelect = 0;
          break;
        case Controller::tTranslate:
          delete mpTranslate;
          mpTranslate = 0;
      	default: break;
      }    	
    break;
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
					mController.addCommand(mpTranslate);
          mpTranslate = 0;
          break;
        default: break;
      }
      break;
    default: setMouseState(msIdle); break;
  }
  
  changeCursor();
  mController.setCanChangeTool(true);
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


