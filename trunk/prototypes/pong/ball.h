/*
 */
#ifndef Pong_Ball_hh
#define Pong_Ball_hh

#include <cassert>
#include "Point.h"
#include "Vect.h"

namespace Pong
{
using namespace realisim::math;

class Ball
{
public:
  Ball();
  Ball(const Ball&);
  virtual ~Ball() {;}
  
  virtual void draw() const;
  virtual double getAngle() const {return mAngle;}
  virtual double getMass() const {return mMass;}
  virtual const Point3d& getPosition() const {return mPosition;}
  virtual double getRadius() const {return mRadius;}
  virtual const Vector3d& getVelocity() const {return mVelocity;}
  const Ball& operator= (const Ball&) {assert(0);}
  virtual void setAngle(double a) {mAngle = a;}
  virtual void setPosition(const Point3d& p) {mPosition = p;}
  virtual void setVelocity(const Vector3d& v) {mVelocity = v;}
  
protected:
  double mRadius;
  double mMass;
  Point3d mPosition;
  double mAngle;
  Vector3d mVelocity;
//double elasticity
//double friction  
};
}

#endif