/*
 *  Sprite.cpp
 */

#include <algorithm>
#include "Camera.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "3d/Sprite.h"
#include "math/Vect.h"

using namespace realisim;
using namespace math;
using namespace treeD;
using namespace std;

Sprite::Sprite() :
 mTexture(),
 mSubTextureRect(),
 mGLTexCoord(),
 mState(sStatic),
 mAnchor(aCenter),
 m2dPosition(0),
 m2dPositioningOn(false),
 mAnimationTimer(),
 mAnimationDuration(),
 mIsLooping(false),
 mFrameSize()
{}

Sprite::Sprite(const Sprite& s) :
 mTexture(s.getTexture()),
 mSubTextureRect(s.getSubTextureRect()),
 mGLTexCoord(s.getGLTexCoord()),
 mState(s.getState()),
 mAnchor(s.getAnchorPoint()),
 m2dPosition(s.get2dPosition()),
 m2dPositioningOn(s.is2dPositioningOn()),
 mAnimationTimer(s.getAnimationTimer()),
 mAnimationDuration(s.getAnimationDuration()),
 mIsLooping(false),
 mFrameSize(s.getFrameSize())
{}

Sprite::~Sprite()
{}

//-----------------------------------------------------------------------------
void Sprite::draw(const Camera& c) const
{
  //il n'y a rien a dessiner si l'animation est terminé et que le looping
  //n'est pas activé
  if(getState() == sAnimatingDone)
    return;
    
  glPushMatrix();
  glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
  
  QRectF texCoord = getGLTexCoord();
  
  if(getState() == sAnimating)
  {
    QSizeF s = getTexture().getSize();
    QSizeF fs(getFrameSize().width() / s.width(),
    getFrameSize().height() / s.height()); //normalized frame size
    
    int totalNumFrames = fabs(getGLTexCoord().height()) /
      fs.height();
    
  	int ms = mAnimationTimer.elapsed();
    if(isLooping())
      ms = ms % mAnimationDuration;
    else
    {
      ms = min(ms, mAnimationDuration);
      ms == mAnimationDuration ? setState(sAnimatingDone) : setState(sAnimating);
    }
    
    /*puisque le système de coordonnées GL commence en bas a gauche,
      le frame 0 serait en bas, nous voulons qu'il soit en haut, alors
      on soustrait le frame courant du nombre total de frames.*/
    int currentFrameIndex = totalNumFrames - 
      ms / (double)mAnimationDuration * totalNumFrames;
    float currentFramePos = fs.height() * currentFrameIndex;
      
    texCoord = QRectF(
      QPointF(fabs(getGLTexCoord().topLeft().x()), 
        currentFramePos),
        QSizeF(fs.width(), -fs.height()) );
  }

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  float a = texCoord.bottomLeft().x();
  float b = texCoord.bottomLeft().y();
  a = texCoord.bottomRight().x();
  b = texCoord.bottomRight().y();

	if(is2dPositioningOn())
  {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    
    glPushMatrix();
    gluLookAt(0, 0, 5,
      0, 0, 0,
      0, 1, 0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, c.getWindowInfo().getWidth(),
      0, c.getWindowInfo().getHeight());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glTranslated(getOriginTranslation().getX(),
      getOriginTranslation().getY(),
      getOriginTranslation().getZ());

    /*afin d'avoir le (0, 0, z) dans le coin supérieur gauche de l'écran, on 
      fait le calcul suivant*/
    int yPos = c.getWindowInfo().getHeight() - 
      m2dPosition.getY();
    glTranslated(m2dPosition.getX(), yPos, m2dPosition.getZ());

    glBindTexture(GL_TEXTURE_2D, getTexture().getTextureId());
    glBegin(GL_QUADS);
    glTexCoord2d(texCoord.bottomLeft().x(), texCoord.bottomLeft().y());
    glVertex2d(0.0, 0.0);
    glTexCoord2d(texCoord.bottomRight().x(), texCoord.bottomRight().y());
    glVertex2d(getSubTextureRect().width(), 0.0);
    glTexCoord2d(texCoord.topRight().x(), texCoord.topRight().y());
    glVertex2d(getSubTextureRect().width(), getSubTextureRect().height());
    glTexCoord2d(texCoord.topLeft().x(), texCoord.topLeft().y());
    glVertex2d(0.0, getSubTextureRect().height());
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopMatrix();
    
    glPopAttrib();
  }
  else 
  {
    glTranslated(getOriginTranslation().getX(),
      getOriginTranslation().getY(),
      getOriginTranslation().getZ());
    glBindTexture(GL_TEXTURE_2D, getTexture().getTextureId());
    glBegin(GL_QUADS);
    glTexCoord2d(texCoord.bottomLeft().x(), texCoord.bottomLeft().y());
    glVertex2d(0.0, 0.0);
    glTexCoord2d(texCoord.bottomRight().x(), texCoord.bottomRight().y());
    glVertex2d(1.0, 0.0);
    glTexCoord2d(texCoord.topRight().x(), texCoord.topRight().y());
    glVertex2d(1.0, 1.0);
    glTexCoord2d(texCoord.topLeft().x(), texCoord.topLeft().y());
    glVertex2d(0.0, 1.0);
    glEnd();
    
    //dessine le frame en debug  
#ifndef NDEBUG
  glDisable(GL_TEXTURE_2D);
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT | GL_LINE_BIT);
  glLineWidth(2.0);
  glColor3ub(255, 255, 255);
  glPolygonMode(GL_FRONT, GL_LINE);
  glBegin(GL_QUADS);
  glVertex2d(0.0, 0.0);
  glVertex2d(1.0, 0.0);
  glVertex2d(1.0, 1.0);
  glVertex2d(0.0, 1.0);
  glEnd();
  glPopAttrib();
#endif

  }

  glPopAttrib();
  glPopMatrix();
}

//-----------------------------------------------------------------------------
Vector3d Sprite::getOriginTranslation() const
{
  Vector3d r(1.0, 1.0, 0);
  if(is2dPositioningOn())
    r.setXYZ(getSubTextureRect().width(), getSubTextureRect().height(), 0.0);
  switch (getAnchorPoint())
  {
    case aBottomLeft: r.setX(0.0); r.setY(0.0); break;
    case aBottomCenter: r.setX(-0.5 * r.getX()); r.setY(0.0); break;
    case aBottomRight: r.setX(-1.0 * r.getX()); r.setY(0.0); break;
    case aCenterLeft: r.setX(0.0); r.setY(-0.5 * r.getY()); break;
    case aCenter: r.setX(-0.5 * r.getX()); r.setY(-0.5 * r.getY()); break;
    case aCenterRight: r.setX(-1.0 * r.getX()); r.setY(-0.5 * r.getY()); break;
    case aTopLeft: r.setX(0.0); r.setY(-1.0 * r.getY()); break;
    case aTopCenter: r.setX(-0.5 * r.getX()); r.setY(-1.0 * r.getY()); break;
    case aTopRight: r.setX(-1.0 * r.getX()); r.setY(-1.0 * r.getY()); break;
    default:break;
  }
  return r;
}

//-----------------------------------------------------------------------------
void Sprite::startAnimation()
{ 
  setState(sAnimating);
  mAnimationTimer.start();
}

//-----------------------------------------------------------------------------
void Sprite::setFrameSize(QSize fs)
{ mFrameSize = fs; }

//-----------------------------------------------------------------------------
void Sprite::setTexture(const Texture& t)
{ setTexture(t, QRect(QPoint(0, 0), t.getSize())); }

//-----------------------------------------------------------------------------
void Sprite::setTexture(const Texture& t, QRect r)
{
  mTexture = t;
  
  /*Le systeme d'axe d'openGL et de Qt sont inversé sur l'axe y.
    C'est pourquoi on fait 1 - r.topLeft().y() / s.height() pour
    obtenir la coordonné gl du coin supérieur droit. De plus,
    afin de pouvoir utiliser un QRect pour stocker le rectangle
    en coordonné gl, la hauteur du rectangle doit être négative.*/
  QSizeF s = mTexture.getSize();
  QPointF tl = QPointF(r.topLeft().x() / s.width(),
    1 - r.topLeft().y() / s.height());
  QSizeF br = QSizeF(r.width() / s.width(),
    -(r.height() / s.height()));

  mSubTextureRect = r;
  mGLTexCoord.setTopLeft(tl);
  mGLTexCoord.setSize(br);
}

//-----------------------------------------------------------------------------
void Sprite::startAnimation(double d, bool l, QSize s)
{
  setAnimationDuration(d);
  setAsLooping(l);
  setFrameSize(s);
  startAnimation();
}

//-----------------------------------------------------------------------------
void Sprite::stopAnimation()
{ setState(sStatic); }

