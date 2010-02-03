/*
 *  GameObject.cpp
 *  Realisim
 *
 *  Created by Pierre-Olivier Beaudoin on 09-11-16.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "GameObject.h"
#include <qgl.h>

using namespace Pong;

GameObject::GameObject() : mMass(1.0), mAngle(0), mVelocity(0.0),
  mToGlobal(),
  mToLocal(),
  mPos(0.0),
  mSprite()
{}

GameObject::GameObject(const GameObject& iG) : mMass(iG.getMass()),
  mAngle(iG.getAngle()),
  mVelocity(iG.getVelocity()),
  mToGlobal(iG.getTransformationToGlobal()),
  mToLocal(iG.getTransformationToLocal()),
  mPos(iG.getPosition()),
  mSprite(iG.getSprite())
{}

GameObject::~GameObject(){}

double GameObject::getAngle() const
{ return mAngle; }

//angle est en radian
void GameObject::setAngle(double iA)
{ mAngle = iA; }

//---Ball-----------------------------------------------------------------------
Ball::Ball() : GameObject(),
  mRadius(0.30) // centimeter
{ setMass(1.0); }

Ball::Ball(const Ball& b) : GameObject(b),
  mRadius(b.getRadius())
{ 
  setMass(b.getMass()); //kg 
}

void Ball::draw(const Camera& c) const
{
  glPushAttrib(GL_POINT_BIT);
  glPushMatrix();
  
  glMultMatrixd(getTransformationToGlobal().getPtr());
  Point3d l = getPosition() * getTransformationToLocal();
  glTranslated(l.getX(), l.getY(), l.getZ());
  glRotated(getAngle() * kRadianToDegree, 0.0, 0.0, 1.0);
  glScaled(2*getRadius(), 2*getRadius(), 2*getRadius());
  mSprite.draw(c);
  
  glPopMatrix();
  glPopAttrib();
}

void Ball::setRadius(double r)
{ mRadius = r; }

//--- Player -------------------------------------------------------------------
Player::Player(QString n) : GameObject(),
  mName(n),
  mLifes(3),
  mId(0),
  mSize(1.0, 0.1), //cm par cm
  mType(tComputer),
  mIsEliminated(false)
{
  setMass(2.0);
}

Player::Player(const Player& p) : GameObject(p),
  mName(p.getName()),
  mLifes(p.getLifes()),
  mId(p.getId()),
  mSize(p.getSize()),
  mType(p.getType())
{}

void Player::draw(const Camera& c) const
{
  double w = getSize().width() / 2.0;
  double h = getSize().height() / 2.0;
  
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT);
  glColor3ub(255, 255, 255);
  glPolygonMode(GL_FRONT, GL_LINE);
  
  glPushMatrix();
  glMultMatrixd(getTransformationToGlobal().getPtr());
  Point3d l = getPosition() * getTransformationToLocal();
  glTranslated(l.getX(), l.getY(), l.getZ());
  glRotated(getAngle() * kRadianToDegree, 0.0, 0.0, 1.0);
  glBegin(GL_QUADS);
  glVertex3d(-w, -h, 0);
  glVertex3d( w, -h, 0);
  glVertex3d( w,  h, 0);
  glVertex3d(-w,  h, 0);
  glEnd();
  glPopMatrix();
  
  glPopAttrib();
}

//--- Net-----------------------------------------------------------------------
Net::Net() : GameObject(),
  mSize(1.8, 0.2) //in meters
{ setMass(0);}

void Net::draw(const Camera& c) const
{
  double w = getSize().width() / 2.0;
  double h = getSize().height() / 2.0;
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT);

    glPushMatrix();
    glMultMatrixd(getTransformationToGlobal().getPtr());
    glColor3ub(0, 255, 255);
    glPolygonMode(GL_FRONT, GL_LINE);
    glBegin(GL_QUADS);
    glVertex3d(-w, -h, 0);
    glVertex3d( w, -h, 0);
    glVertex3d( w,  h, 0);
    glVertex3d(-w,  h, 0);
    glEnd();
    glPopMatrix();
    
  glPopAttrib();
}

//--- Collision-----------------------------------------------------------------
Collision::Collision() : GameObject()
{
  mSprite.setAnchorPoint(Sprite::aBottomCenter);
  mSprite.startAnimation(400, false, QSize(72, 96));
}

void Collision::draw(const Camera& c) const
{
  glPushMatrix();
  glMultMatrixd(getTransformationToGlobal().getPtr());
  
#ifndef NDEBUG
  glPushAttrib(GL_CURRENT_BIT);
  glColor3ub(0.0, 255, 0.0);
  glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(mN.getX(), mN.getY(), mN.getZ());
  glEnd();
  glPopAttrib();
#endif
  
  //le dessin de la collision a une rotation implicite de 90deg... il
  //faut donc l'enlever
  glRotated((getAngle() - PI_SUR_2) * kRadianToDegree, 0.0, 0.0, 1.0);
  glTranslated(0.0, -0.3, 0.0);
  glScaled(1.0, 1.5, 0.0);
  mSprite.draw(c);
  glPopMatrix();
}
  
  