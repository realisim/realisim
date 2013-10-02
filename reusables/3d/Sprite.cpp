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
 mAnchor(s.anchorPoint()),
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
  mAnchor = s.anchorPoint();
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
void Sprite::draw() const
{

	Vector3d trans(0.0);
  switch ( anchorPoint() )
  {
    case aBottomLeft: trans.setXYZ(0.5, 0.5, 0.0); break;
    case aBottomCenter: trans.setXYZ(0.0, 0.5, 0.0); break;
    case aBottomRight: trans.setXYZ(-0.5, 0.5, 0.0); break;
    case aCenterLeft: trans.setXYZ(0.5, 0.0, 0.0); break;
    case aCenter: trans.setXYZ(0.0, 0.0, 0.0); break;
    case aCenterRight: trans.setXYZ(-0.5, 0.0, 0.0); break;
    case aTopLeft: trans.setXYZ(0.5, -0.5, 0.0); break;
    case aTopCenter: trans.setXYZ(0.0, -0.5, 0.0); break;
    case aTopRight: trans.setXYZ(-0.5, -0.5, 0.0); break;
    default: break;
  }
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glDisable( GL_LIGHTING );
  
	glPushMatrix();
  glScaled( getFrameWidth(), getFrameHeight(), 0.0 );
  glTranslated( trans.getX(), trans.getY(), trans.getZ() );
  
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
  glEnable(GL_LIGHTING);
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
  Point2d bottomLeft( topLeft.x() / (double)getTexture().width(),
  	( getTexture().height() - bottomRight.y() ) / 
    (double)getTexture().height() );
  Point2d topRight(
  	bottomLeft.x() + getFrameWidth() / (double)getTexture().width(),
    bottomLeft.y() + getFrameHeight() / (double)getTexture().height() );
  return Rectangle(bottomLeft, topRight);
}

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


