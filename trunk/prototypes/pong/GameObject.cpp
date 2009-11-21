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
  mTransformation() 
{}

GameObject::GameObject(const GameObject& iG) : mMass(iG.getMass()),
  mAngle(iG.getAngle()),
  mVelocity(iG.getVelocity()),
  mTransformation(iG.getTransformation()) 
{}

GameObject::~GameObject(){}

double GameObject::getAngle() const
{ return mAngle; }

//angle est en radian
void GameObject::setAngle(double iA)
{ mAngle = iA; }

//---Ball-----------------------------------------------------------------------
Ball::Ball() : GameObject(),
  mRadius(0.10) // 10 centimeter
{
  setMass(1.0);
}

Ball::Ball(const Ball& b) : GameObject(b),
  mRadius(b.getRadius())
{
  setMass(0.2); //kg
}

void Ball::draw() const
{
  glPushAttrib(GL_POINT_BIT);
  glPushMatrix();
  glMultMatrixd(getTransformation().getPtr());
//  glTranslated(mPosition.getX(), mPosition.getY(), mPosition.getZ());
  glRotated(getAngle() * kRadianToDegree, 0.0, 0.0, 1.0);
  glBegin(GL_LINE_LOOP);
 
  float degInRad;
  for (int i=0; i <= 360; i+=6)
  {
    degInRad = i*kDegreeToRadian;
    glVertex2f(cos(degInRad)*getRadius(),
      sin(degInRad)*getRadius());
  }
  glEnd();
  
  glPointSize(4.0);
  glBegin(GL_POINTS);
  glVertex2d(0.0, 0.7 * getRadius());
  glEnd();
  glPopMatrix();
  glPopAttrib();
}

//--- Player -------------------------------------------------------------------
Player::Player(QString n) : GameObject(),
  mName(n),
  mLifes(3),
  mId(0),
  mSize(0.6, 0.1), //cm par cm
  mType(tHuman)
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

void Player::draw() const
{
  double w = getSize().width() / 2.0;
  double h = getSize().height() / 2.0;
  
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT);
  glColor3ub(255, 255, 255);
  glPolygonMode(GL_FRONT, GL_LINE);
  
  glPushMatrix();
  glMultMatrixd(getTransformation().getPtr());
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

void Net::draw() const
{
  double w = getSize().width() / 2.0;
  double h = getSize().height() / 2.0;
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT);

    glPushMatrix();
    glMultMatrixd(getTransformation().getPtr());
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