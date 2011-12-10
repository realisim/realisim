#ifndef SpaceTime_Engine_Engine_h
#define SpaceTime_Engine_Engine_h

#include "math/Matrix4x4.h"
#include "math/Point.h"
#include "math/Vect.h"
class QByteArray;
#include <QObject>
#include <QPoint>
#include <QString>
#include <QTimer>
class QTimerEvent;
#include <vector>
#include <map>
#include <set>

namespace SpaceTime { class Object;}
namespace SpaceTime { class Ship;}
namespace SpaceTime { class AstronomicalBody;}
namespace SpaceTime { class Engine;}

using namespace realisim;
  using namespace math;
using namespace std;

//-----------------------------------------------------------------------------
class SpaceTime::Object
{
  public: 
    Object() : mName("n/a"), mAcceleration(0.0), mMass(0.0),
      mDensity(1.0), mForce(0.0),
      mSpeed(0.0), mTransformation() {}
    virtual ~Object() {}
    
    const Vector3d& getAcceleration() const {return mAcceleration;}
    double getDensity() const {return mDensity;}
    double getMass() const {return mMass;}
    const Vector3d& getForce() const {return mForce;}
    const QString& getName() const {return mName;}
    const Matrix4d& getTransformation() const {return mTransformation;}
    const Vector3d& getSpeed() const {return mSpeed;}
    void setAcceleration(const Vector3d& iA) {mAcceleration = iA;}
    void setDensity(double iD) {mDensity = iD;}
    void setMass(double iM) {mMass = iM;}
    void setForce(const Vector3d& iF) {mForce = iF;}
    void setName(const QString& iN) {mName = iN;}
    void setTransformation(const Matrix4d& iM) {mTransformation = iM;}
    void setSpeed(const Vector3d& iS) {mSpeed = iS;} 
    
  protected:
    QString mName;
    Vector3d mAcceleration; //km / s
    double mMass; //kg
    double mDensity; //Kg/km3
    Vector3d mForce;
    Vector3d mSpeed;
    Matrix4d mTransformation;
// boundingbox?
// boundingSphere      
};

//-----------------------------------------------------------------------------
class SpaceTime::Ship : public SpaceTime::Object
{
  public:
    Ship() : Object(), mRoll(0.0) {}
    virtual ~Ship() {}
    double getRoll() const {return mRoll;}
    void setRoll(double iR) {mRoll = iR;}
    
  protected:
    double mRoll;    	
};

//-----------------------------------------------------------------------------
class SpaceTime::AstronomicalBody : public SpaceTime::Object
{
public:
  AstronomicalBody() : Object(), mType(tPlanet), mRadius(0.0), mPath() {}
  virtual ~AstronomicalBody(){};
  
  enum type{tBlackHole, tComet, tMoon, tPlanet, tStar, tNumberOfType};

  void addToPath(const Point3d&);
  const vector<Point3d>& getPath() const {return mPath;}
  double getRadius() const {return mRadius;}
  type getType() const {return mType;}
  void resetPath() {mPath.clear();}
  void setRadius(double iR) {mRadius = iR;}
  void setType(type iT) {mType = iT;}
  
protected:
  type mType;
  double mRadius;
  vector<Point3d> mPath;
};

//-----------------------------------------------------------------------------
class SpaceTime::Engine : public QObject
{
public:
	Engine();
  virtual ~Engine();
  
  class Client
  {
  	public:
      Client(Engine& iE) : mEngine(iE) {;}
      virtual ~Client() {}
      
      enum message{mFrameReady, mPaused, mPlaying, mRegistered,
        mStateChanged, mUnregistered};
      virtual void call(message) = 0;
      virtual Engine& getEngine() {return mEngine;}
      virtual const Engine& getEngine() const {return mEngine;}
      
    protected:
      Engine& mEngine;
  };
  
  enum state {sSimulating, sPlaying, sPaused};
  enum key{kA, kD, kE, kQ, kS, kW, kShift, kNumKeys};
  
virtual void fonctionBidon();
  virtual void fromBinary(const QByteArray&);
  virtual void generateAstronomicalBodies(long long);
  virtual void generateTestBodies1();
  virtual void generateTestBodies2(long long);
  virtual QString getAndClearLastErrors() const;
  virtual const vector<AstronomicalBody*>& getAstronomicalBodies() const;
  virtual const vector<AstronomicalBody*> getAstronomicalBodies(const Point3d&, double) const;
  virtual const vector<AstronomicalBody*> getAstronomicalBodies(const Point3d&, double, double) const;
  virtual long long getNumberOfCycles() const {return mCycles;}
  virtual long long getRadiusOfGeneration() const {return mRadiusOfGeneration;}
  virtual long long getSpaceRadius() const {return mSpaceRadius;}
  virtual const Ship& getShip() const {return mShip;}
  virtual state getState() const {return mState;}
  virtual void goToState(state);
  virtual bool isDebugging() const {return mIsDebugging;}
  virtual void ignoreMouseMove(QPoint);
  virtual void keyPressed(key);
  virtual void keyReleased(key);  
  virtual void mouseMoved(int, int);
  virtual void registerClient(Client*);
  virtual void setAsDebugging(bool iD) {mIsDebugging = iD;}
  virtual void setRadiusOfGeneration(long long iR) {mRadiusOfGeneration = iR;}
  virtual void setSpaceRadius(long long iR) {mSpaceRadius = iR;}
virtual void step();
  virtual QByteArray toBinary() const;
  virtual void unregisterClient(Client*);
  virtual void unregisterAllClients();
  
  
protected:
	struct Mouse
  {
  	Mouse() : mDeltaX(0), mDeltaY(0) {}
    QPoint mPos;
    int mDeltaX;
    int mDeltaY;
  };
  
  struct CollisionPair
  {
    CollisionPair(long long i1, long long i2) : mIndex1(i1 < i2 ? i1 : i2),
      mIndex2(i2 >= i1 ? i2 : i1) {}
      
    bool operator<(const CollisionPair& iLhs) const
    {	
      if(mIndex1 < iLhs.mIndex1) return true;
      if(mIndex1 > iLhs.mIndex1) return false;
      
      if(mIndex2 < iLhs.mIndex2) return true;
      if(mIndex2 > iLhs.mIndex2) return false;
      
      return false;
    }
    
    long long mIndex1;
    long long mIndex2;
  };
  
  virtual void addAstronomicalBody(AstronomicalBody*, long long);
  virtual void addError(const QString&);
  virtual void applyForces();
  virtual void callClients(Client::message);
  virtual void deleteAllAstronomicalBodies();
  virtual void deleteMarkedBodies();
  virtual void detectCollisions();
  virtual void explodeAstronomicalBody(AstronomicalBody*);
  virtual QString generateName(int iNumLetter) const;
  virtual void simulate(int);
  virtual void handleCollisions(int);
  virtual void handleUserInput();
  virtual void merge(AstronomicalBody*, AstronomicalBody*);
  virtual void moveBodies(int);
  virtual double normalizedRandom() const;
  virtual void play(int);
  virtual void refreshSortedPositions();
  virtual void solveCollisionsOverlap();
  virtual void timerEvent(QTimerEvent*);
  
  vector<Client*> mClients;
	vector<AstronomicalBody*> mAstronomicalBodies;
//map<QString, long long> mNameToId;
  multimap<Point3d, long long> mSortedPositions;
  Ship mShip;
  bool mKeyboard[kNumKeys];
  Mouse mMouse;
  bool mIsDebugging;
  state mState;
  int mTimerId;
  QTimer mTimer;
  long long mCycles;
  long long mRadiusOfGeneration;
  long long mSpaceRadius;
  mutable QString mErrors;
  set<CollisionPair> mCollisions;
  set<AstronomicalBody*> mMarkToDelete;
};


#endif
