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
    Object() : mName("n/a"), mAcceleration(0.0), mMass(0.0), mForce(0.0),
      mSpeed(0.0), mTransformation() {}
    virtual ~Object() {}
    
    const Vector3d& getAcceleration() const {return mAcceleration;}
    double getMass() const {return mMass;}
    const Vector3d& getForce() const {return mForce;}
    const QString& getName() const {return mName;}
    const Matrix4d& getTransformation() const {return mTransformation;}
    const Vector3d& getSpeed() const {return mSpeed;}
    void setAcceleration(const Vector3d& iA) {mAcceleration = iA;}
    void setMass(double iM) {mMass = iM;}
    void setForce(const Vector3d& iF) {mForce = iF;}
    void setName(const QString& iN) {mName = iN;}
    void setTransformation(const Matrix4d& iM) {mTransformation = iM;}
    void setSpeed(const Vector3d& iS) {mSpeed = iS;} 
    
  protected:
    QString mName;
    Vector3d mAcceleration; //km / s
    double mMass; //kg
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
  AstronomicalBody() : Object(), mType(tPlanet), mRadius(0.0) {}
  virtual ~AstronomicalBody(){};
  
  enum type{tBlackHole, tComet, tMoon, tPlanet, tStar, tNumberOfType};

  double getRadius() const {return mRadius;}
  type getType() const {return mType;}
  void setRadius(double iR) {mRadius = iR;}
  void setType(type iT) {mType = iT;}
  
protected:
  type mType;
  double mRadius;
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
  
  enum state {sGenerating, sPlaying, sPaused};
  enum key{kA, kD, kE, kQ, kS, kW, kNumKeys};
  
  virtual void fromBinary(const QByteArray&);
  virtual void generateAstronomicalBodies(long long);
  virtual void generateTestBodies();
  virtual QString getAndClearLastErrors() const;
  virtual const vector<AstronomicalBody*>& getAstronomicalBodies() const;
  virtual const vector<AstronomicalBody*> getAstronomicalBodies(const Point3d&, double) const;
  virtual const vector<AstronomicalBody*> getAstronomicalBodies(const Point3d&, double, double) const;
  virtual long long getNumberOfCycles() const {return mCycles;}
  virtual long long getRadiusOfGeneration() const {return mRadiusOfGeneration;}
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
  
  virtual void addAstronomicalBody(AstronomicalBody*, long long);
  virtual void addError(const QString&);
  virtual void callClients(Client::message);
  virtual void deleteAllAstronomicalBodies();
  virtual QString generateName(int iNumLetter) const;
  virtual void generatingStep(int);
  virtual void handleUserInput();
  virtual void playingStep(int);
  virtual void refreshSortedPositions();
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
  mutable QString mErrors;
};


#endif
