/*
 *  Sprite.cpp
 */

#include <algorithm>
#include "Camera.h"
#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "3d/Sprite.h"
#include "3d/Utilities.h"
#include "math/Vect.h"

using namespace realisim;
using namespace math;
using namespace treeD;
using namespace std;

Sprite::Sprite() :
 mTexture(),
 mAnchor(aCenter),
 mTimer(),
 mAnimationDuration(1000),
 mIsLooping(true),
 mFrameSize(0),
 mRect(),
 mNumberOfFrames(1),
 mFrameGrid(1, 1),
 mCurrentFrameIndex(0)
{ startAnimation(); }

Sprite::Sprite(const Sprite& s) :
 mTexture(s.getTexture()),
 mAnchor(s.getAnchorPoint()),
 mTimer(s.getTimer()),
 mAnimationDuration(s.getAnimationDuration()),
 mIsLooping(s.isLooping()),
 mFrameSize(s.getFrameSize()),
 mRect(s.getRect()),
 mNumberOfFrames(s.getNumberOfFrames()),
 mFrameGrid(s.getFrameGrid()),
 mCurrentFrameIndex(s.getCurrentFrameIndex())
{}

Sprite& Sprite::operator=(const Sprite& s)
{
  mTexture = s.getTexture();
  mAnchor = s.getAnchorPoint();
  mTimer = s.getTimer();
  mAnimationDuration = s.getAnimationDuration();
  mIsLooping = s.isLooping();
  mFrameSize = s.getFrameSize();
  mRect = s.getRect();
  mNumberOfFrames = s.getNumberOfFrames();
  mFrameGrid = s.getFrameGrid();
  mCurrentFrameIndex = s.getCurrentFrameIndex();
  return *this;
}

Sprite::~Sprite()
{}

//-----------------------------------------------------------------------------
QImage Sprite::asQImage() const
{ return getTexture().asQImage().copy( getRect() ); }

//-----------------------------------------------------------------------------
void Sprite::draw() const
{
	
	Vector2d trans = getTranslation();
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  
	glPushMatrix();
  glTranslated( trans.x(), trans.y(), 0.0 );
  glScaled( getFrameWidth(), getFrameHeight(), 0.0 );
  
  Point2d o( -0.5 );
  Vector2d s( 1.0 );
  math::Rectangle tc = getFrameTextureCoordinate();
  glEnable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, getTexture().getId() );
  glBegin(GL_QUADS);
  glTexCoord2d( tc.bottomLeft().x(), tc.bottomLeft().y() );
  glVertex2d( o.x(), o.y() );
  glTexCoord2d( tc.topLeft().x(), tc.topLeft().y() );
  glVertex2d( o.x(), o.y() + s.y() );
  glTexCoord2d( tc.topRight().x(), tc.topRight().y() );
  glVertex2d( o.x() + s.x(), o.y() + s.y() );
  glTexCoord2d( tc.bottomRight().x(), tc.bottomRight().y() );
  glVertex2d( o.x() + s.x(), o.y() );
  glEnd();
	glDisable( GL_TEXTURE_2D );

  glPopMatrix();
  glDisable(GL_BLEND);
}

//-----------------------------------------------------------------------------
/*Retourne l'index de la frame courante pour l'animation.*/
int Sprite::getCurrentFrameIndex() const
{
	if( getNumberOfFrames() > 1 )
  {
  	int e = getTimer().elapsed();
    double t;
    t = isLooping() ? 
    	( e % getAnimationDuration() ) / (double)getAnimationDuration() :
      e / (double)getAnimationDuration();
    t = isLooping() ? t : min(t, 1.0);
    mCurrentFrameIndex = floor(t * ( getNumberOfFrames() ));
	}
  else mCurrentFrameIndex = 0;
	return mCurrentFrameIndex;
}

//-----------------------------------------------------------------------------
/*Retourne les coordonnées de texture opengl pour le frame courant de
  l'animation*/
math::Rectangle Sprite::getFrameTextureCoordinate() const
{
  int i = getCurrentFrameIndex();
  Vector2i fg = getFrameGrid();
  int row = i / fg.x();
  int column = i % fg.x();
	QPoint topLeft = getRect().topLeft();
  topLeft.rx() += column * getFrameSize().x();
  topLeft.ry() += row * getFrameSize().y();
  QPoint bottomRight( topLeft.x() + getFrameSize().x(),
  	topLeft.y() + getFrameSize().y() );
  
  //on convertit le tout pour openGL
  /*Le petit +1 pour bottomLeft est le résultat de la définition de QRect.
    voir la doc. */
  Point2d bottomLeft( topLeft.x() / (double)getTexture().width(),
  	( getTexture().height() + 1 - bottomRight.y() ) / 
    (double)getTexture().height() );
  Point2d topRight(
  	bottomLeft.x() + getFrameWidth() / (double)getTexture().width(),
    bottomLeft.y() + getFrameHeight() / (double)getTexture().height() );
  return Rectangle(bottomLeft, topRight);
}

//-----------------------------------------------------------------------------
/*Retourne la translation en pixel appliquée au sprite en fonction de son point 
  d'ancrage.*/
Vector2d Sprite::getTranslation() const
{
	Vector2d r(0.0);

  switch ( getAnchorPoint() )
  {
    case aBottomLeft: r.set(0.5, 0.5); break;
    case aBottomCenter: r.set(0.0, 0.5); break;
    case aBottomRight: r.set(-0.5, 0.5); break;
    case aCenterLeft: r.set(0.5, 0.0); break;
    case aCenter: r.set(0.0, 0.0); break;
    case aCenterRight: r.set(-0.5, 0.0); break;
    case aTopLeft: r.set(0.5, -0.5); break;
    case aTopCenter: r.set(0.0, -0.5); break;
    case aTopRight: r.set(-0.5, -0.5); break;
    default: break;
  }
	r.set( r.x() * getFrameSize().x(), r.y() * getFrameSize().y() );
  return r ;
}

//-----------------------------------------------------------------------------
bool Sprite::isAnimationDone() const
{ return mTimer.elapsed() > getAnimationDuration(); }

//-----------------------------------------------------------------------------
bool Sprite::isValid() const
{ return getTexture().isValid() && getRect().isValid(); }

//-----------------------------------------------------------------------------
void Sprite::set(const Texture& t)
{ set(t, QRect(0, 0, t.width(), t.height() ));}

//-----------------------------------------------------------------------------
void Sprite::set(const Texture& t, QRect r)
{
  //pour l'instant, les sprites ne supportent que les textures 2d.
  assert(t.getType() == Texture::t2d);
  mTexture = t;
  setRect( r );
}

//-----------------------------------------------------------------------------
void Sprite::set(QImage i)
{
	Texture t;
  t.set(i);
  t.setFilter( GL_NEAREST, GL_NEAREST );
  t.setWrapMode( GL_CLAMP );
	set(t, QRect(0, 0, t.width(), t.height() ));
}

//-----------------------------------------------------------------------------
void Sprite::setFrameGrid( int ix, int iy )
{
	mFrameGrid = Vector2i(ix, iy);
  mFrameSize.set( getRect().width() / ix,
  	getRect().height() / iy );
} 

//-----------------------------------------------------------------------------
void Sprite::setRect( const QRect& iR )
{
	mRect = iR;
	setFrameGrid( getFrameGrid().x(), getFrameGrid().y() );
} 

//-----------------------------------------------------------------------------
void Sprite::startAnimation()
{ mTimer.start(); mCurrentFrameIndex = 0; }


