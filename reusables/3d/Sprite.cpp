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
 mFrameSize(),
 mIsFullScreen(false)
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
 mIsLooping(s.isLooping()),
 mFrameSize(s.getFrameSize()),
 mIsFullScreen(s.isFullScreen())
{}

Sprite& Sprite::operator=(const Sprite& s)
{
  mTexture = s.getTexture();
  mSubTextureRect =s.getSubTextureRect();
  mGLTexCoord = s.getGLTexCoord();
  mState = s.getState();
  mAnchor = s.getAnchorPoint();
  m2dPosition = s.get2dPosition();
  m2dPositioningOn = s.is2dPositioningOn();
  mAnimationTimer = s.getAnimationTimer();
  mAnimationDuration = s.getAnimationDuration();
  mIsLooping = s.isLooping();
  mFrameSize = s.getFrameSize();
  mIsFullScreen = s.isFullScreen();
  return *this;
}

Sprite::~Sprite()
{}

//-----------------------------------------------------------------------------
void Sprite::draw(const Camera& c, bool iPicking /*= false*/) const
{
  //il n'y a rien a dessiner si l'animation est terminé et que le looping
  //n'est pas activé
  if(getState() == sAnimatingDone)
    return;
    
  glPushMatrix();
  
  QRectF texCoord = getGLTexCoord();
  
  if(getState() == sAnimating)
  {
    QSizeF s(getTexture().getSizeX(), getTexture().getSizeY() );
    //QSizeF s(getSubTextureRect().width(), getSubTextureRect().height());
    QSizeF fs(getFrameSize().width() / s.width(),
    getFrameSize().height() / s.height()); //normalized frame size
    
  	int ms = mAnimationTimer.elapsed();
    if(isLooping())
      ms = ms % mAnimationDuration;
    else
    {
      ms = min(ms, mAnimationDuration);
      if (ms == mAnimationDuration)
      {
        setState(sAnimatingDone);
      }
    }
    
    int maxFrameIndex = fabs(getGLTexCoord().height()) /
      fs.height() - 1;
    /*puisque le système de coordonnées GL commence en bas a gauche,
      le frame 0 serait en bas, nous voulons qu'il soit en haut, alors
      on soustrait le frame courant du nombre total de frames.*/
    int currentFrameIndex = maxFrameIndex - 
      ms / (double)mAnimationDuration * maxFrameIndex;
      
    /*Puisqu'on fonctionne avec le coin supérieur droit comme point de
      départ (un drôle de choix, mais il semblerait que c'Est comme 
      ça que j'ai fait) voici l'équation pour obtenir la composante
      y de la coordonnée de texture.*/
    float currentFramePos = fs.height() + fs.height() * currentFrameIndex;
      
    /*L'utilisation du systeme de coordonné Qt et OpenGl commence à être
      très confus dans cette classe... La taille en y doit etre négative.
      Je crois que c'est dû au fait qu'on a pris le coin supérieur
      gauche (topLeft, voir Sprite::setTexture(const Texture& t, QRect r)).*/
    texCoord = QRectF(
      QPointF(fabs(getGLTexCoord().topLeft().x()), 
        currentFramePos),
        QSizeF(fs.width(), -fs.height()) );
  }

	if(!iPicking)
  {
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  else
  {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
  }

  
  float a = texCoord.bottomLeft().x();
  float b = texCoord.bottomLeft().y();
  a = texCoord.bottomRight().x();
  b = texCoord.bottomRight().y();

	if(is2dPositioningOn() && !isFullScreen())
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

		if(getState() == sStatic)
    {
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
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
      glBindTexture(GL_TEXTURE_2D, getTexture().getTextureId());
      glBegin(GL_QUADS);
      glTexCoord2d(texCoord.bottomLeft().x(), texCoord.bottomLeft().y());
      glVertex2d(0.0, 0.0);
      glTexCoord2d(texCoord.bottomRight().x(), texCoord.bottomRight().y());
      glVertex2d(getFrameSize().width(), 0.0);
      glTexCoord2d(texCoord.topRight().x(), texCoord.topRight().y());
      glVertex2d(getFrameSize().width(), getFrameSize().height());
      glTexCoord2d(texCoord.topLeft().x(), texCoord.topLeft().y());
      glVertex2d(0.0, getFrameSize().height());
      glEnd();
      glBindTexture(GL_TEXTURE_2D, 0); 
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopMatrix();
    
    glPopAttrib();
  }
  else if(!isFullScreen())
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
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //dessine le frame en debug  
#ifndef NDEBUG
  glDisable(GL_TEXTURE_2D);
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT | GL_LINE_BIT);
  glLineWidth(2.0);
  glColor3ub(255, 255, 255);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_QUADS);
  glVertex2d(0.0, 0.0);
  glVertex2d(1.0, 0.0);
  glVertex2d(1.0, 1.0);
  glVertex2d(0.0, 1.0);
  glEnd();
  glPopAttrib();
#endif
  }
  else //it is fullscreen
  {
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, c.getWindowInfo().getWidth(),
      0, c.getWindowInfo().getHeight());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBindTexture(GL_TEXTURE_2D, getTexture().getTextureId());
    glBegin(GL_QUADS);
    glTexCoord2d(texCoord.bottomLeft().x(), texCoord.bottomLeft().y());
    glVertex2d(0.0, 0.0);
    glTexCoord2d(texCoord.bottomRight().x(), texCoord.bottomRight().y());
    glVertex2d(c.getWindowInfo().getWidth(), 0.0);
    glTexCoord2d(texCoord.topRight().x(), texCoord.topRight().y());
    glVertex2d(c.getWindowInfo().getWidth(), c.getWindowInfo().getHeight());
    glTexCoord2d(texCoord.topLeft().x(), texCoord.topLeft().y());
    glVertex2d(0.0, c.getWindowInfo().getHeight());
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glEnable(GL_DEPTH_TEST);
  }

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
void Sprite::set2dPosition(const math::Vector3i& p)
{
  m2dPosition.setXYZ((double)p.getX(),
                     (double)p.getY(),
                     (double)p.getZ());
}

//-----------------------------------------------------------------------------
void Sprite::set2dPosition(const int iX, const int iY)
{ set2dPosition(Vector3d((double)iX, (double)iY, 0.0)); }

//-----------------------------------------------------------------------------
void Sprite::set2dPosition(const int iX, const int iY, double iZ)
{ set2dPosition(Vector3d((double)iX, (double)iY, iZ)); }

//-----------------------------------------------------------------------------
void Sprite::setFrameSize(QSize fs)
{ mFrameSize = fs; }

//-----------------------------------------------------------------------------
void Sprite::setTexture(const Texture& t)
{ setTexture(t, QRect(0, 0, t.getSizeX(), t.getSizeY() ));}

//-----------------------------------------------------------------------------
void Sprite::setTexture(const Texture& t, QRect r)
{
  //pour l'instant, les sprites ne supportent que les textures 2d.
  //assert(t.getType() == Texture::t2d);
  mTexture = t;
  
  /*Le systeme d'axe d'openGL et de Qt sont inversé sur l'axe y.
    C'est pourquoi on fait 1 - r.topLeft().y() / s.height() pour
    obtenir la coordonné gl du coin supérieur droit. De plus,
    afin de pouvoir utiliser un QRect pour stocker le rectangle
    en coordonné gl, la hauteur du rectangle doit être négative.*/
  QSizeF s(mTexture.getSizeX(), mTexture.getSizeY() );
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

