//

#ifndef Pong_Player_hh
#define Pong_Player_hh

#include <cassert>
#include "MathUtils.h"
#include "Matrix4x4.h"
#include "Point.h"
#include <QSize>
#include <QString>
#include "Vect.h"

namespace Pong
{
using namespace realisim;
  using namespace math;

class Player
{
public:
  enum type {tComputer, tHuman};
  Player(QString);
  Player(const Player&);
  virtual ~Player(){;}
  
  virtual void draw() const;
  virtual void move(const Vector3d& v) {mTransformation.setTranslation(getPosition()+v);}
  virtual double getAngle() const {return mAngle;}
  virtual const int getId() const {return mId;}
  virtual const double getMass() const {return mMass;}
  virtual const QString& getName() const {return mName;}
  virtual const Point3d getPosition() const {return mTransformation.getTranslation();}
  virtual const QSizeF& getSize() const {return mSize;}
  virtual const Matrix4d& getTransformation() const {return mTransformation;}
  virtual type getType() const {return mType;}
  virtual void setAngle(double a) {mAngle = a;}
  virtual void setId(int i) {mId = i;}
  virtual void setMass(double m) {mMass = m;}
  virtual void setName(QString n) {mName = n;}
  virtual void setPosition(const Point3d& p) {mTransformation.setTranslation(p);}
  virtual void setSize(const QSizeF& s) {mSize = s;}
  virtual void setTransformation(const Matrix4d& m) {mTransformation = m;}
  virtual void setType(type t) {mType = t;}

protected:
  QString mName;
  unsigned int mId;
  QSizeF mSize;
  double mAngle;
  Matrix4d mTransformation;
  double mMass;
//double elasticity
//double friction
  type mType;
};

}

#endif
