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
 mState(sStatic),
 mAnchor(aCenter),
 mTimer(),
 mAnimationDuration(),
 mIsLooping(false)
{}

Sprite::Sprite(const Sprite& s) :
 mTexture(s.getTexture()),
 mState(s.getState()),
 mAnchor(s.anchorPoint()),
 mTimer(s.getTimer()),
 mAnimationDuration(s.getAnimationDuration()),
 mIsLooping(s.isLooping())
{}

Sprite& Sprite::operator=(const Sprite& s)
{
  mTexture = s.getTexture();
  mState = s.getState();
  mAnchor = s.anchorPoint();
  mTimer = s.getTimer();
  mAnimationDuration = s.getAnimationDuration();
  mIsLooping = s.isLooping();
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
  glScaled( getTexture().width(), getTexture().height(), 0.0 );
  glTranslated( trans.getX(), trans.getY(), trans.getZ() );
  utilities::drawRectangle2d( getTexture(), Point2d( -0.5 ), Vector2d( 1.0 ) );
  glPopMatrix();
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

//-----------------------------------------------------------------------------
void Sprite::animate()
{ setState(sAnimating); }

//-----------------------------------------------------------------------------
void Sprite::set(const Texture& t)
{ set(t, QRect(0, 0, t.width(), t.height() ));}

//-----------------------------------------------------------------------------
void Sprite::set(const Texture& t, QRect r)
{
  //pour l'instant, les sprites ne supportent que les textures 2d.
  assert(t.getType() == Texture::t2d);
  mTexture = t;
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
void Sprite::setState( state iS )
{
	switch (iS) 
  {
    case sAnimating:
    	mTimer.start();
      mState = sAnimating;
    break;
    case sStatic:
    	mState = sStatic;
    break;
    default: break;
  }
}

//-----------------------------------------------------------------------------
//void Sprite::startAnimation(double d, bool l, QSize s)
//{
//  setAnimationDuration(d);
//  setAsLooping(l);
//  startAnimation();
//}
//
//-----------------------------------------------------------------------------
//void Sprite::stopAnimation()
//{ setState(sStatic); }

