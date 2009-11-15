/*
 */

#include "ball.h"
#include "MathDef.h"
#include <qgl.h>

using namespace Pong;
using namespace realisim::math;

Ball::Ball() : mRadius(0.10), // 10 centimeter
  mMass(1.0), //1Kg
  mPosition(0.0),
  mVelocity(0.0)
{}

Ball::Ball(const Ball& b) : mRadius(b.getRadius()),
  mMass(b.getMass()),
  mPosition(b.getPosition()),
  mVelocity(b.getVelocity())
{}

void Ball::draw() const
{
  glPushAttrib(GL_POINT_BIT);
  glPushMatrix();
  glTranslated(mPosition.getX(), mPosition.getY(), mPosition.getZ());
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
