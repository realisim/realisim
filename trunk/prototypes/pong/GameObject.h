/*
*/
#ifndef Pong_GameObject_hh
#define Pong_GameObject_hh

#include <cassert>
#include "MathUtils.h"
#include "Matrix4x4.h"
#include "Point.h"
#include <QSize>
#include <QString>
#include "Vect.h"

namespace Pong 
{
using namespace realisim::math;

class GameObject
{
public:
  GameObject();
  GameObject(const GameObject&);
  virtual ~GameObject();
  
  virtual void draw() const = 0;
  virtual double getAngle() const;
  virtual double getMass() const {return mMass;}
  virtual const Point3d getPosition() const {return mTransformation.getTranslation();}
  virtual const Matrix4d& getTransformation() const {return mTransformation;}
  virtual const Vector3d& getVelocity() const {return mVelocity;}
  virtual void setAngle(double);
  virtual void setMass(const double m) {mMass = m;}
  virtual void setTransformation(const Matrix4d& m) {mTransformation = m;}
  virtual void setPosition(const Point3d& p) {mTransformation.setTranslation(p);}
  virtual void setVelocity(const Vector3d& v) {mVelocity = v;}
  
protected:
  double mMass;
  double mAngle;
  Vector3d mVelocity;
  Matrix4d mTransformation;
  //double elasticity
  //double friction  
};

//--- Ball ---------------------------------------------------------------------
class Ball : public GameObject
{
public:
  Ball();
  Ball(const Ball&);
  virtual ~Ball() {;}

  virtual void draw() const;
  virtual double getRadius() const {return mRadius;}
  const Ball& operator= (const Ball&) {assert(0);}

protected:
  double mRadius;
};

//--- Player--------------------------------------------------------------------
class Player : public GameObject
{
public:
  enum type {tComputer, tHuman};
  Player(QString);
  Player(const Player&);
  virtual ~Player(){;}
  
  virtual void draw() const;
  virtual const int getId() const {return mId;}
  virtual unsigned int getLifes() const {return mLifes;}
  virtual const QString& getName() const {return mName;}
  virtual const QSizeF& getSize() const {return mSize;}

  virtual type getType() const {return mType;}
  virtual void setId(int i) {mId = i;}
  virtual void setLifes(unsigned int i) {mLifes = i;}
  virtual void setName(QString n) {mName = n;}
  virtual void setSize(const QSizeF& s) {mSize = s;}
  virtual void setType(type t) {mType = t;}

protected:
  QString mName;
  unsigned int mLifes;
  unsigned int mId;
  QSizeF mSize;
  type mType;
};

class Net : public GameObject
{
public:
  Net();
  Net(const Net&) {assert(0);}
  virtual ~Net(){;}
  
  virtual void draw() const;
  virtual const QSizeF& getSize() const {return mSize;}
  virtual void setSize(const QSizeF& s) {mSize = s;}
  
protected:
  QSizeF mSize;
};
}

#endif
 
 

