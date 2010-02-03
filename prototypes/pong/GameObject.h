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
namespace realisim {namespace treeD{class Camera;}}
#include "Sprite.h"
#include "Vect.h"


namespace Pong 
{
using namespace realisim;
using namespace math;
using namespace treeD;

/*

 getPosition return the position in GLOBAL coordinates
 setPosition requires a GLOBAL coordinate.
 
 mPos: player's position stored in local coordinates.
*/
class GameObject
{
public:
  GameObject();
  GameObject(const GameObject&);
  virtual ~GameObject();
  
  virtual void draw(const Camera&) const = 0;
  virtual double getAngle() const;
  virtual double getMass() const {return mMass;}
  virtual const Point3d getPosition() const {return mPos*getTransformationToGlobal();}
  virtual const Sprite& getSprite() const {return mSprite;}
  virtual const Matrix4d& getTransformationToGlobal() const {return mToGlobal;}
  virtual const Matrix4d& getTransformationToLocal() const {return mToLocal;}
  virtual const Vector3d& getVelocity() const {return mVelocity;}
  virtual void setAngle(double);
  virtual void setMass(const double m) {mMass = m;}
  virtual void setTransformationToGlobal(const Matrix4d& m)
    {mToGlobal = m; mToLocal = m; mToLocal.inverse();}
  virtual void setTransformationToLocal(const Matrix4d& m)
    {mToLocal = m; mToGlobal = m; mToGlobal.inverse();}
  virtual void setPosition(const Point3d& p)
    {mPos = p*getTransformationToLocal();}
  virtual void setVelocity(const Vector3d& v) {mVelocity = v;}
  virtual void setTexture(const Texture& t) {mSprite.setTexture(t);}
  virtual void setTexture(const Texture& t, QRect r) {mSprite.setTexture(t, r);}
  
protected:
  double mMass;
  double mAngle;
  Vector3d mVelocity;
  Matrix4d mToGlobal;
  Matrix4d mToLocal;
  Point3d mPos;
  //double elasticity
  //double friction  
  Sprite mSprite;
};

//--- Ball ---------------------------------------------------------------------
class Ball : public GameObject
{
public:
  Ball();
  Ball(const Ball&);
  virtual ~Ball() {;}

  virtual void draw(const Camera&) const;
  virtual double getRadius() const {return mRadius;}
  const Ball& operator= (const Ball&) {assert(0);}
  virtual void setRadius(double);

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
  
  virtual void draw(const Camera&) const;
  virtual const int getId() const {return mId;}
  virtual unsigned int getLifes() const {return mLifes;}
  virtual const QString& getName() const {return mName;}
  virtual const QSizeF& getSize() const {return mSize;}
  virtual bool isEliminated() const {return mIsEliminated;}

  virtual type getType() const {return mType;}
  virtual void setAsEliminated(bool e = true) {mIsEliminated = e;}
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
  bool mIsEliminated;
};

class Net : public GameObject
{
public:
  Net();
  Net(const Net&) {assert(0);}
  virtual ~Net(){;}
  
  virtual void draw(const Camera&) const;
  virtual const QSizeF& getSize() const {return mSize;}
  virtual void setSize(const QSizeF& s) {mSize = s;}
  
protected:
  QSizeF mSize;
};

class Collision : public GameObject
{
public:
  Collision();
  virtual ~Collision(){;}
  
  virtual void animate() {mSprite.startAnimation();}
  virtual void draw(const Camera&) const;
  bool isDone() const {return mSprite.getState() == Sprite::sAnimatingDone;}
virtual void setNormal(const Vector3d& n) {mN = n;}
  
protected:
Vector3d mN;
};
}

#endif
 
 

