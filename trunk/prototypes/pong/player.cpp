//

#include "player.h"
#include <qgl.h>

using namespace Pong;

Player::Player(QString n) : mName(n), 
  mId(0),
  mSize(0.6, 0.1), //cm par cm
  mTransformation(),
  mMass(0.5), //kg
  mType(tHuman)
{}

Player::Player(const Player& p) :
  mName(p.getName()), 
  mId(p.getId()),
  mSize(p.getSize()),
  mTransformation(p.getTransformation()),
  mMass(p.getMass()),
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
  //glTranslated(getPosition().getX(), getPosition().getY(), getPosition().getZ());
glRotated(mAngle * kRadianToDegree, 0.0, 0.0, 1.0);
  glBegin(GL_QUADS);
  glVertex3d(-w, -h, 0);
  glVertex3d( w, -h, 0);
  glVertex3d( w,  h, 0);
  glVertex3d(-w,  h, 0);
  glEnd();
  glPopMatrix();
  
  glPopAttrib();
}
